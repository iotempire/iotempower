#!/usr/bin/env node
"use strict";

const fs = require("fs");
const path = require("path");
const crypto = require("crypto");

const action = process.argv[2] || "status";
const marker = "IOTEMPOWER_NODE_RED_ADMIN_AUTH";
const markerEnd = "IOTEMPOWER_NODE_RED_ADMIN_AUTH_END";
const oldDefaultHash = "$2b$08$W5LDP3eTaIYjz5iJkKVwMu9JDg3cPF" +
    "MUvBypMCmYA3fpjYQlzFC4e";
const username = "admin";

const userDir = process.env.IOTEMPOWER_NODE_RED_USER_DIR ||
    path.join(process.env.HOME || "", ".node-red");
const settingsFile = process.env.IOTEMPOWER_NODE_RED_SETTINGS_FILE ||
    process.argv[3] ||
    path.join(userDir, "settings.js");
const settingsDir = path.dirname(settingsFile);
const credentialsFile = process.env.IOTEMPOWER_NODE_RED_CREDENTIALS_FILE ||
    path.join(settingsDir, "iotempower-admin-credentials");
const hashFile = process.env.IOTEMPOWER_NODE_RED_PASSWORD_HASH_FILE ||
    path.join(settingsDir, "iotempower-admin-password.hash");

function exists(file) {
    try {
        fs.accessSync(file, fs.constants.F_OK);
        return true;
    } catch (err) {
        return false;
    }
}

function readText(file) {
    return fs.readFileSync(file, "utf8");
}

function readFirstLine(file) {
    try {
        return readText(file).split(/\r?\n/, 1)[0].trim();
    } catch (err) {
        if (err && err.code === "ENOENT") {
            return "";
        }
        throw err;
    }
}

function modeOf(file) {
    try {
        return fs.statSync(file).mode & 0o777;
    } catch (err) {
        if (err && err.code === "ENOENT") {
            return null;
        }
        throw err;
    }
}

function formatMode(mode) {
    return mode === null ? "missing" : mode.toString(8).padStart(3, "0");
}

function chmodPrivate(file) {
    try {
        fs.chmodSync(file, 0o600);
    } catch (err) {
        // Best effort for filesystems without POSIX mode support.
    }
}

function removeIfExists(file) {
    try {
        fs.unlinkSync(file);
    } catch (err) {
        if (!err || err.code !== "ENOENT") {
            throw err;
        }
    }
}

function writePrivateAtomic(file, content) {
    fs.mkdirSync(path.dirname(file), { recursive: true, mode: 0o700 });
    const tmp = `${file}.tmp-${process.pid}-${Date.now()}`;
    try {
        fs.writeFileSync(tmp, content, { mode: 0o600 });
        chmodPrivate(tmp);
        fs.renameSync(tmp, file);
        chmodPrivate(file);
    } catch (err) {
        removeIfExists(tmp);
        throw err;
    }
}

function loadBcrypt() {
    const candidates = ["bcryptjs"];
    if (process.env.IOTEMPOWER_LOCAL) {
        candidates.push(path.join(process.env.IOTEMPOWER_LOCAL, "nodejs", "node_modules", "bcryptjs"));
        candidates.push(path.join(process.env.IOTEMPOWER_LOCAL, "nodejs", "node_modules", "node-red", "node_modules", "bcryptjs"));
    }
    if (process.env.IOTEMPOWER_NODE_RED_BCRYPTJS_PATH) {
        candidates.unshift(process.env.IOTEMPOWER_NODE_RED_BCRYPTJS_PATH);
    }

    let lastError;
    for (const candidate of candidates) {
        try {
            return require(candidate);
        } catch (err) {
            lastError = err;
        }
    }

    const detail = lastError && lastError.message ? ` ${lastError.message}` : "";
    throw new Error(`Cannot load bcryptjs for IoTempower Node-RED adminAuth.${detail}`);
}

function isIdentifierStart(char) {
    return /[A-Za-z_$]/.test(char);
}

function isIdentifierPart(char) {
    return /[A-Za-z0-9_$]/.test(char);
}

function readQuotedToken(text, start) {
    const quote = text[start];
    let index = start + 1;
    let value = "";

    while (index < text.length) {
        const char = text[index];
        if (char === "\\") {
            if (index + 1 < text.length) {
                value += text[index + 1];
                index += 2;
                continue;
            }
            index += 1;
            break;
        }
        if (char === quote) {
            index += 1;
            break;
        }
        value += char;
        index += 1;
    }

    return { token: { type: "string", value }, index };
}

function tokenizeJsSettings(text) {
    const tokens = [];
    let index = 0;

    while (index < text.length) {
        const char = text[index];
        const next = text[index + 1];

        if (/\s/.test(char)) {
            index += 1;
            continue;
        }

        if (char === "/" && next === "/") {
            index += 2;
            while (index < text.length && !/[\r\n]/.test(text[index])) {
                index += 1;
            }
            continue;
        }

        if (char === "/" && next === "*") {
            index += 2;
            while (index < text.length && !(text[index] === "*" && text[index + 1] === "/")) {
                index += 1;
            }
            index = index < text.length ? index + 2 : index;
            continue;
        }

        if (char === "'" || char === "\"" || char === "`") {
            const quoted = readQuotedToken(text, index);
            tokens.push(quoted.token);
            index = quoted.index;
            continue;
        }

        if (isIdentifierStart(char)) {
            const start = index;
            index += 1;
            while (index < text.length && isIdentifierPart(text[index])) {
                index += 1;
            }
            tokens.push({ type: "identifier", value: text.slice(start, index) });
            continue;
        }

        tokens.push({ type: "punct", value: char });
        index += 1;
    }

    return tokens;
}

function tokenValue(tokens, index) {
    return tokens[index] ? tokens[index].value : "";
}

function isSettingToken(token, setting) {
    return token &&
        (token.type === "identifier" || token.type === "string") &&
        token.value === setting;
}

function isObjectLiteralBoundary(value) {
    return value === "{" || value === ",";
}

function isDotAssignment(tokens, index, setting) {
    return tokens[index] &&
        tokens[index].type === "identifier" &&
        tokens[index].value === setting &&
        tokenValue(tokens, index - 1) === "." &&
        tokenValue(tokens, index + 1) === "=";
}

function isBracketAssignment(tokens, index, setting) {
    return tokens[index] &&
        tokens[index].type === "string" &&
        tokens[index].value === setting &&
        tokenValue(tokens, index - 1) === "[" &&
        tokenValue(tokens, index + 1) === "]" &&
        tokenValue(tokens, index + 2) === "=";
}

function isObjectProperty(tokens, index, setting) {
    if (isSettingToken(tokens[index], setting) &&
            tokenValue(tokens, index + 1) === ":" &&
            isObjectLiteralBoundary(tokenValue(tokens, index - 1))) {
        return true;
    }

    return tokens[index] &&
        tokens[index].type === "string" &&
        tokens[index].value === setting &&
        tokenValue(tokens, index - 1) === "[" &&
        tokenValue(tokens, index + 1) === "]" &&
        tokenValue(tokens, index + 2) === ":" &&
        isObjectLiteralBoundary(tokenValue(tokens, index - 2));
}

function hasJsSetting(text, setting) {
    const tokens = tokenizeJsSettings(text);
    return tokens.some((token, index) =>
        isDotAssignment(tokens, index, setting) ||
        isBracketAssignment(tokens, index, setting) ||
        isObjectProperty(tokens, index, setting));
}

function readStringSetting(text, setting, defaultValue) {
    const tokens = tokenizeJsSettings(text);
    for (let index = 0; index < tokens.length; index += 1) {
        if (isDotAssignment(tokens, index, setting) && tokens[index + 2] && tokens[index + 2].type === "string") {
            return tokens[index + 2].value;
        }
        if (isBracketAssignment(tokens, index, setting) && tokens[index + 3] && tokens[index + 3].type === "string") {
            return tokens[index + 3].value;
        }
        if (isObjectProperty(tokens, index, setting)) {
            const valueIndex = tokenValue(tokens, index + 1) === ":" ? index + 2 : index + 3;
            if (tokens[valueIndex] && tokens[valueIndex].type === "string") {
                return tokens[valueIndex].value;
            }
        }
    }
    return defaultValue;
}

function hasManagedAuth(text) {
    const start = text.indexOf(marker);
    if (start === -1) {
        return false;
    }
    const end = text.indexOf(markerEnd, start + marker.length);
    if (end === -1) {
        return false;
    }
    return hasJsSetting(text.slice(start, end + markerEnd.length), "adminAuth");
}

function readSettingsInfo() {
    if (!exists(settingsFile)) {
        return {
            exists: false,
            managed: false,
            customAdminAuth: false,
            legacyDefaultHash: false,
        };
    }
    const text = readText(settingsFile);
    return {
        exists: true,
        managed: hasManagedAuth(text),
        customAdminAuth: hasJsSetting(text, "adminAuth"),
        legacyDefaultHash: text.includes(oldDefaultHash),
    };
}

function readHttpAdminRoot() {
    if (!exists(settingsFile)) {
        return "/nodered";
    }
    const text = readText(settingsFile);
    return readStringSetting(text, "httpAdminRoot", "/nodered");
}

function readPasswordFromCredentials() {
    const content = readText(credentialsFile);
    const match = content.match(/^Password:[ \t]*(.*)$/m);
    return match ? match[1] : "";
}

function fileSummary(file) {
    const mode = modeOf(file);
    return {
        exists: mode !== null,
        mode,
        modeOk: mode === 0o600,
    };
}

function consistency() {
    const settings = readSettingsInfo();
    const credentials = fileSummary(credentialsFile);
    const hash = fileSummary(hashFile);
    const hashValue = readFirstLine(hashFile);
    let password = "";
    let recoveryReadable = false;
    let hashMatchesRecovery = "not-checkable";

    if (credentials.exists) {
        try {
            password = readPasswordFromCredentials();
            recoveryReadable = password.length > 0;
        } catch (err) {
            recoveryReadable = false;
        }
    }

    if (settings.managed && recoveryReadable && hashValue) {
        try {
            hashMatchesRecovery = loadBcrypt().compareSync(password, hashValue) ? "yes" : "no";
        } catch (err) {
            hashMatchesRecovery = "error";
        }
    }

    const ready = settings.managed &&
        credentials.exists &&
        hash.exists &&
        credentials.modeOk &&
        hash.modeOk &&
        recoveryReadable &&
        hashMatchesRecovery === "yes";

    return {
        settings,
        credentials,
        hash,
        recoveryReadable,
        hashMatchesRecovery,
        ready,
    };
}

function generatePassword() {
    return crypto.randomBytes(24).toString("base64")
        .replace(/\+/g, "-")
        .replace(/\//g, "_")
        .replace(/=+$/g, "");
}

function credentialsContent(password) {
    return [
        "IoTempower Node-RED admin credentials",
        `Username: ${username}`,
        `Password: ${password}`,
        `URL path: ${readHttpAdminRoot()}`,
        "",
        "Keep this file private.",
        "Use `iot nodered-password reset` or `iot nodered-password set` to rotate it.",
        "",
    ].join("\n");
}

function requireManagedSettings() {
    const settings = readSettingsInfo();
    if (!settings.exists) {
        throw new Error(`Node-RED settings file does not exist: ${settingsFile}`);
    }
    if (!settings.managed) {
        if (settings.customAdminAuth) {
            throw new Error(`Node-RED settings has custom adminAuth; leaving it unchanged: ${settingsFile}`);
        }
        throw new Error(`IoTempower-managed Node-RED adminAuth is not configured in ${settingsFile}`);
    }
}

function writePassword(password) {
    if (!password || !/\S/.test(password)) {
        throw new Error("Password must not be empty.");
    }
    if (/[\r\n]/.test(password)) {
        throw new Error("Password must be a single line.");
    }
    if (password !== password.trim()) {
        throw new Error("Password must not start or end with whitespace.");
    }
    requireManagedSettings();
    const bcrypt = loadBcrypt();
    const hash = bcrypt.hashSync(password, 8);
    writePrivateAtomic(credentialsFile, credentialsContent(password));
    try {
        writePrivateAtomic(hashFile, `${hash}\n`);
    } catch (err) {
        removeIfExists(credentialsFile);
        throw err;
    }
    return password;
}

function ensureFiles() {
    requireManagedSettings();
    const state = consistency();
    if (state.ready) {
        chmodPrivate(credentialsFile);
        chmodPrivate(hashFile);
        return { changed: false, password: readPasswordFromCredentials() };
    }
    const password = writePassword(generatePassword());
    return { changed: true, password };
}

function printStatus() {
    const state = consistency();
    console.log(`Node-RED settings: ${settingsFile}`);
    console.log(`IoTempower-managed auth: ${state.settings.managed ? "yes" : "no"}`);
    if (!state.settings.managed && state.settings.customAdminAuth) {
        console.log("Admin auth type: custom");
    }
    console.log(`Username: ${state.settings.managed ? username : "n/a"}`);
    console.log(`Credential file: ${credentialsFile}`);
    console.log(`Credential file mode: ${formatMode(state.credentials.mode)}${state.credentials.modeOk ? " (ok)" : ""}`);
    console.log(`Hash file: ${hashFile}`);
    console.log(`Hash file mode: ${formatMode(state.hash.mode)}${state.hash.modeOk ? " (ok)" : ""}`);
    console.log(`Recovery file has password: ${state.recoveryReadable ? "yes" : "no"}`);
    console.log(`Recovery/hash consistent: ${state.hashMatchesRecovery}`);
    console.log(`Legacy default hash present: ${state.settings.legacyDefaultHash ? "yes" : "no"}`);
    console.log(`Status: ${state.ready ? "ready" : "needs attention"}`);
    return state.ready ? 0 : 1;
}

function showPassword() {
    const state = consistency();
    if (!state.settings.managed) {
        throw new Error("IoTempower-managed Node-RED auth is not configured.");
    }
    if (!state.credentials.exists) {
        throw new Error(`Credential file does not exist: ${credentialsFile}. Run: iot nodered-password reset`);
    }
    if (!state.credentials.modeOk) {
        throw new Error(`Refusing to print credentials because ${credentialsFile} mode is ${formatMode(state.credentials.mode)}, expected 600.`);
    }
    const password = readPasswordFromCredentials();
    if (!password) {
        throw new Error(`Credential file has no readable Password line: ${credentialsFile}. Run: iot nodered-password reset`);
    }
    console.log(`Username: ${username}`);
    console.log(`Password: ${password}`);
}

function readStdin() {
    return fs.readFileSync(0, "utf8").replace(/\r?\n$/, "");
}

try {
    if (action === "has-setting") {
        const setting = process.argv[4];
        if (!setting) {
            throw new Error("Missing setting name.");
        }
        process.exit(exists(settingsFile) && hasJsSetting(readText(settingsFile), setting) ? 0 : 1);
    } else if (action === "status") {
        process.exit(printStatus());
    } else if (action === "is-managed") {
        process.exit(readSettingsInfo().managed ? 0 : 1);
    } else if (action === "show") {
        showPassword();
    } else if (action === "ensure-files") {
        const result = ensureFiles();
        if (result.changed) {
            console.log(`Created Node-RED admin credential recovery file: ${credentialsFile}`);
        }
    } else if (action === "reset") {
        writePassword(generatePassword());
        console.log("Node-RED admin password reset.");
        console.log(`Username: ${username}`);
        console.log(`Password file: ${credentialsFile}`);
    } else if (action === "set") {
        writePassword(readStdin());
        console.log("Node-RED admin password updated.");
        console.log(`Username: ${username}`);
        console.log(`Password file: ${credentialsFile}`);
    } else {
        throw new Error(`Unknown action: ${action}`);
    }
} catch (err) {
    console.error(err && err.message ? err.message : String(err));
    process.exit(1);
}
