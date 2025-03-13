var term = require( 'terminal-kit' ).terminal;
var child_process = require('child_process');


function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function term_grab() {
    term.grabInput( { mouse: 'motion' } ) ;
    term.hideCursor(true);    
}

function term_release() {
    term.hideCursor( false );
    term.grabInput( false ) ;
}

function terminate() {
    term_release();
    // Add a 100ms delay, so the terminal will be ready when the process effectively exit, preventing bad escape sequences drop
    setTimeout( function() { process.exit() ; } , 100 ) ;
}

async function choice(items, pre_select=0, pad_last=0) {

    term_grab();

    var button_padding = 4;
    var top_left = "┌";
    var top_bar = "─";
    var top_right = "┐";
    var bar_left = "│";
    var bar_right = "│";
    var bottom_left = "└";
    var bottom_bar = "─";
    var bottom_right = "┘";

    // get longest word
    var longest = 0;
    for(var i=0; i<items.length; i++) {
        if(items[i][0].length > longest) longest = items[i][0].length;
    }
    longest += 6; // Add button inside padding

    // create formatted_items
    var menu_height = 0;
    var formatted_items = [];
    for(var i=0; i<items.length; i++) {
        var padlength = longest - items[i][0].length;
        var fstr = 
                " ".repeat(button_padding) + top_left + top_bar.repeat(longest) 
                    + top_right + "\n" +
                " ".repeat(button_padding) + bar_left + " ".repeat(Math.floor(padlength/2)) + items[i][0] 
                    + " ".repeat(Math.floor((padlength+1)/2)) + bar_right + "\n" +
                " ".repeat(button_padding) + bottom_left + bottom_bar.repeat(longest) 
                    + bottom_right + " ".repeat(button_padding);
        formatted_items.push(fstr);
        menu_height += 3;
    }
    for( var i=0; i<pad_last; i++) {
        // add line before last
        formatted_items[items.length-1] = "\n" + formatted_items[items.length-1];
        menu_height ++;
    }

    // if negative count back from the last
    if(pre_select<0) pre_select = formatted_items.length + pre_select;

    const extra_lines = 2
    menu_height += extra_lines;

    // make space for menu (term.nextLine seems to be buggy or not scroll correctly)
    for(var i=0; i<menu_height; i++)
        term("\n");
    // go up again
    term.previousLine(menu_height);

    function clean_up() {
        term_release();
        term.up(menu_height-extra_lines); // why is extra_lines needed here?
        term.eraseDisplayBelow();
    }

    term.singleColumnMenu( formatted_items, 
        { 
            leftPadding: "",
            selectedLeftPadding: "",
            cancelable: true,
            selectedIndex: pre_select,
            itemMaxWidth: 2*(button_padding+1)+longest,
            extraLines: extra_lines,
            style: term.bgBrightWhite.black,
            selectedStyle: term.bgBlue.black,
            submittedStyle: term.bgWhite.black,
            exitOnUnexpectedKey: true, // catch other keys
            continueOnSubmit: false
        } ,
        function( error , response ) {
            // term( '\n' ).eraseLineAfter.green(
            //     "#%s" ,
            //     response.selectedIndex
            // );
            clean_up();
            if(response == undefined) {
                term(error);
                term("\n");
            } 
            else if(response.unexpectedKey != undefined) {
                function check_key(name, key, cb) {
                    if(name.toUpperCase() === key.toUpperCase()) {
                        cb();
                        return true;
                    }
                    return false;
                }
                
                var name = response.unexpectedKey;
                for(var i=0; i<items.length; i++) {
                    if(items[i][1] instanceof Array) {
                        for(var k of items[i][1]) {
                            if(check_key(name, k, items[i][2])) return; // match found
                        }
                    } else {
                        if(check_key(name, items[i][1], items[i][2])) return; // match found
                    }
                } // end for
                // not returned, so wo need to end as not matched
                terminate();
            } else {
                if(response.selectedIndex == undefined) terminate();
                else {
                    var cb = items[response.selectedIndex][2];
                    if(cb == undefined) { terminate(); }
                    else { cb(); }
                }
            }
        }
    ) ;
    // Attention: this is Javascript, so execution directly continues here,
    // so everything needs to be done in callbacks and via recursion if
    // deterministic behavior is expected
    // Careful: while menu is running, cursor changes all the time, so
    // nothing can decently be printed (maybe modal)
}

function run_command(command, directory = null) {
    // check for Mac OSX and skip '-c' (command) flag because the Darwin/BSD version
    // of the script tool does not use this flag!
    // Darwin/BSD style: script [-aeFfkqr] [-t time] [file [command ...]]
    // therefore, /dev/null at the end is handled as input to the command and not as redirection of file.
    var opsys = process.platform;
    if (opsys == "darwin") {
        var args = ['-q', '-e', '/dev/null', command ];  
    } else {
        var args = ['-q', '-e', '-c', command, '/dev/null' ];
    }
//    if(directory)
//        cmd = 'cd "' + directory + '"; ' + cmd;
    term.wrap("\nRunning ", command, "\n\n");
    term_release()
    var result = child_process.spawnSync("script", args,
                    { stdio: [0,0,0], cwd: directory });
    term_grab()
    if( result.status == 0) {
        term.green.wrap('\nThe command ', command, ' has completed successful. OK to continue.\n');
        choice([
            ["OK, continue (O, Enter)", "O", terminate]
        ], pre_select=-1);
    } else {
        term.red.wrap('\nAn error occurred running ',command,'. Check output, continue, and eventually try again later.\n');
        choice([
            ["Continue (C, Enter)", "C", terminate]
        ], pre_select=-1);
    }
}


function shell_command(question, command, directory = null) {
    if(question) {
        term.wrap("\n", question, "\n");
        
        choice([
            ["Yes, run " + command + " (Y)", "Y", function() { run_command(command, directory); } ],
            ["No, go back (N)", "N", terminate]
        ], pre_select=-1);
    } else {
        term("\n\n");
        run_command(command, directory);
        term("\n\n");
    }
}

function get_active_dir() {
    var d = process.env.ACTIVE_DIR;
    var fulldir = d;
    if(!d) {
        d = process.cwd();
        fulldir = d;
    }
    return fulldir;
}

function get_short_dir(fulldir) {
    // Strip home directory
    var d = fulldir;
    var home = process.env.HOME;
    if(d.startsWith(home)) d = d.slice(home.length+1);
    return d;
}

function shell_command_in_path(before_path, after_path, command) {
    term.wrap('\n\n', before_path, '\n');
    var fulldir = get_active_dir();
    term.wrap(get_short_dir(fulldir),'\n');
    shell_command(after_path, command, fulldir);
}


function deploy() {
    shell_command_in_path('You are about to deploy from the following path:',
        'Are you sure?', "deploy");
}


function compile() {
    shell_command_in_path('You are about to run compile in the following path:',
        'Are you sure?', "compile");
}

function function_factory_reduce_param( f, param ) {
    return function() {f(param);};
}

function adopt_selection(node_list, original_list, start) {
    var choice_list = []
    const max_entries = 5

    var counter = 1
    for(var index=start; index<start+max_entries && index<node_list.length; index++) {
        choice_list.push([node_list[index] + " (" + counter +")", "" + counter, 
        function_factory_reduce_param(function(selection){
            selection = selection.split(" ")[0];
            var subsel = selection.substring(10);
            shell_command_in_path("You are about to adopt the node "
                + subsel + " from the following path:",
                "Are you sure that this is the right node and path?",
                "adopt " + selection);
        }, original_list[index])]);
        counter ++;
    }
    if(start+max_entries < node_list.length) { // couldn't show all -> add next button
        choice_list.push(["More nodes (M)", "M", 
            function_factory_reduce_param( function(p){ 
                adopt_selection(p[0], p[1], p[2]); 
            }, [node_list, original_list, start + max_entries]) ]);
    }
    if(start > 0) {
        choice_list.push(["Previous nodes (P)", "P", 
        function_factory_reduce_param( function(p){ 
            adopt_selection(p[0], p[1], p[2]); 
        }, [node_list, original_list, Math.max(start - max_entries, 0)]) ]);
    } else {
        choice_list.push(["(B,X) Back", ["B", "X"], terminate]);
    }
    choice(choice_list, pre_select=0);
    
}


function adopt() {
    term.wrap("\nScanning for adoptable nodes... ");
    var result = child_process.spawnSync('dongle', [ 'scan' ], {
//        cwd: fulldir,
        env: process.env,
        stdio: 'pipe',
        encoding: 'utf-8'
    });
    //{ stdio: [0,0,0], cwd: directory });
    var savedOutput = result.stdout.toString().split('\n');;
    term.wrap("ok\n");

    var ap_list = [];
    for (let item of savedOutput) {
        if(item) {
            var space_split = item.split(" ");
            // var strength = space_split[-1]
            item = space_split[0].split("-")
            ap_list.push( "node " + item[2] + ": " 
                + item[3][0] + " long, " 
                + item[4][0] + " short blinks");
        }
    }

    if(ap_list.length > 0) {
        adopt_selection(ap_list, savedOutput, 0)
    } else { // no node to adopt found
        term.red.wrap('\nNo node to adopt found, make sure you have at least one node in adoption mode.\n');
        choice([
            ["Continue (C, Enter)", "C", terminate]
        ], pre_select=-1);
    }
}

function create_node_template() {
    shell_command_in_path("You are about to create a folder called new-node"
        + " to configure a new node. Do not forget to rename this folder after"
        + " this creation. you might have to refresh cloudcmd to see the newly"
        + " created folder after this action. It will be created in the following path:",
        'Are you sure?',
        "create_node_template");
}


function upgrade() {
    shell_command("This will get the newest version of iotempower out of the internet."
        + " Are you sure you want to run this?", "iot upgrade");
}


function initialize_serial() {
    shell_command_in_path('You are about to initialize'
        + ' a device that is locally connected to the gateway via usb or serial'
        + ' from the following path:',
        'Are you sure, and have you connected only one node to the gateway?',
        "initialize serial");
}


function pre_flash_wemos() {
    shell_command('You are about to flash a connected Wemos D1 Mini with'
        + ' a default iotempower firmware. Are you sure, and have you connected '
        + 'exactly one Wemos D1 Mini via USB to the gateway?',
        "pre_flash_wemos");
}


function shell_escape() {
    shell_command_in_path("Starting the shell (bash) now. "
        + "Remember to type exit to leave. Initial path will be:",
        null, "bash");
}

function poweroff() {
    shell_command("This will cleanly shut down the gateway. "
        + "Are you sure you want to shutdown and power off the gateway?",
        "sudo /usr/bin/poweroff");
}

function wifi_config() {
    shell_command("This will open an editor with the WiFi-router configuration."
        + " Please make corresponding changes. You will have to power off and on"
        + " the gateway after this edit to activate the new configuration. "
        + "\nDo you want to continue editing the WiFi-router configuration?",
        "sudo /usr/bin/mcedit /boot/wifi.txt");
}

function iot_system_template(){
    shell_command_in_path("You are about to create a folder called for a new IoT system"
        + " Do not forget to rename this folder after"
        + " this creation. If you are using cloudcmd, you might have to refresh to see the newly"
        + " created folder after this action. It will be created inside the iot-systems folder",
        'Are you sure?',
        "create_system_template");
}

function wifi_setup_systemconf() {
    term("\n\n");
    term
    term.bgBrightWhite.black.brightCyan.wrap( '^+!!!This must be run inside a project folder^: (or its parents) which contains a system.conf file' );
    term("\n");
    shell_command("This will edit system configuration file where you can assign the wifi credentials for specific projects."
        + "\nThis will overwrite the default credentials. It is useful if your project has different wifi credentials than the default ones"
        + "\nDo you want to continue editing your project's WiFi configuration (system.conf)?",
        "wifi_setup_systemconf");
}

function wifi_setup_global() {
    shell_command("This will create a iotempower.conf file that creates global credentials for your systems."
        + "\nIf your accesspoint is an internal wifi chip, this will define it's credentials"
        + "\nThis wull define the SSID and password for wifi by creating a iotempower.conf file."
        + "\nThis wifi credentials will become your default settings. "
        + "\nDo you want to continue editing your IoTempower's wiFi configuration?",
        " wifi_setup_global");
}

function wifi_setup_openwrt() {
    term("\n\n");
    term
    term.bgBrightWhite.black.brightCyan.wrap( '^+!!!Make sure you are connected to your router via Ethernet or Wifi^' );
    term("\n");
    term.bgBrightWhite.black.brightCyan.wrap( '^+!!!Make sure your router has internet connection^' );
    term("\n");
    shell_command("This will create a file that pre configure your gateway credentials."
        + " Please define the SSID and password for your external WiFi-router running OpenWRT."
        + " This Wifi credentials will become your default settings. "
        + "\nDo you want to continue editing your Gateway's WiFi configuration?",
        " wifi_setup_openwrt");
}
function wifi_update_openwrt() {
    shell_command("This will a file that pre configure your gateway credentials."
        + " Please define the SSID and password for your external WiFi-router running OpenWRT."
        + " This Wifi credentials will become your default settings. "
        + "\nDo you want to continue editing your Gateway's WiFi configuration?",
        " wifi_update_openwrt");
}

function advanced(back) {
    choice([
        ["Pre-Flash Wemos D1 Mini (P)", "P", pre_flash_wemos],
        ["Initialize Serial (I)", "I", initialize_serial], 
        // confusing - better through shell ["Compile (C)", "C", compile], 
        ["Upgrade (U)", "U", upgrade],
        ["Create New System (Y)", "Y", iot_system_template],
        ["Shell Escape (S)", "S", shell_escape],
        ["Shutdown/Poweroff (O)", "O", poweroff],
        ["Back (B,X,ESC)", ["B","X"], back?menu_default:terminate]
    ], pre_select=-1, pad_last=1);
}

function advanced_back() {
    advanced(true);
}

function system_configuration(back) {
    choice([
        ["WiFi system credentials setup - system.conf file (W)", "W", wifi_setup_systemconf],
        ["Wifi network initial setup - OpenWRT router (R)", "R",  wifi_setup_openwrt],
        ["Update WiFi Credentials on OpenWRT router (U)", "U",  wifi_update_openwrt],
        ["Set-up wifi gateway on the Raspberry Pi (P)", "P", wifi_config],
        ["Back (B,X,ESC)", ["B","X"], back?menu_default:terminate]
    ], pre_select=-1, pad_last=1);
}
function system_configuration_back() {
    system_configuration(true);
}

function menu_default() {
    //term.clear();
    term("\n\n");
    term
    term.bgBrightWhite.black.brightCyan.wrap( '^+IoT^bempower^: User Menu\n' );
    term("\n");
    term.wrap('Use mouse, arrow keys, enter, or shortcut key to select.\n')
    term("\n");

    choice([
        ["Deploy (D)", "D", deploy], 
        ["Adopt (A)", "A", adopt],
        ["WiFi Network Setup (C)", "C", system_configuration_back],
        ["Create New Node Folder (N)", "N", create_node_template],
        ["Advanced (V)", "V", advanced_back],
        ["Exit (X,ESC)", "X", terminate]
    ], pre_select=0, pad_last=1);
}


/////// main program
switch(process.argv[process.argv.length - 1]) {
    case "deploy":
        deploy();
        break;
    case "adopt":
        adopt();
        break;
    case "system_configuration":
        system_configuration(false);
        break;       
    case "create_node_template":
        create_node_template();
        break;
    case "advanced":
        advanced(false);
        break;
    case "pre_flash_wemos":
        pre_flash_wemos();
        break;
    case "initialize_serial":
        initialize_serial();
        break;
    case "upgrade":
        upgrade();
        break;
    case "wifi_config":
        wifi_config();
        break;
    case "shell":
        shell_escape();
        break;
    case "set_wificredentials_systemconf":
        set_wificredentials_systemconf();
        break;
    case "iot_system_template":
        iot_system_template();
    case "poweroff":
        poweroff();
    default:
        menu_default();
}
