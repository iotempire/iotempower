var term = require( 'terminal-kit' ).terminal ;
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
            selectedStyle: term.bgGreen.black,
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
            if(response.unexpectedKey != undefined) {
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
    var args = ['-q', '-e', '-c', command, '/dev/null' ];
//    if(directory)
//        cmd = 'cd "' + directory + '"; ' + cmd;
    term.wrap("\nRunning ", command, "\n\n");
    var result = child_process.spawnSync("script", args,
                    { stdio: [0,0,0], cwd: directory });
    if( result.status == 0) {
        term.green.wrap('\nThe command ',command,' seems to have completed successful. OK to continue.\n');
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
    }
}


function shell_command_in_path(before_path, after_path, command) {
    term.wrap('\n\n', before_path, '\n');
    var d = process.env.ACTIVE_DIR;
    var fulldir = d;
    if(!d) {
        d = process.cwd();
        fulldir = d;
    }
    // Strip home directory
    var home = process.env.HOME;
    if(d.startsWith(home)) d = d.slice(home.length+1);
    term.wrap(d,'\n');

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


function initialize() {
    shell_command_in_path('You are about to initialize from the following path:',
        'Are you sure and have you successfully put one node in adoption mode?',
        "initialize");
}


function create_node_template() {
    shell_command_in_path('You are about to create a folder called node_template in the following path:',
        'Are you sure?',
        "create_node_template");
}


function upgrade() {
    shell_command("This will get the newest version of ulnoiot out of the internet."
        + " Are you sure you want to run this?", "upgrade");
}


function initialize_serial() {
    shell_command_in_path('You are about to initialize'
        + ' a device that is locally connected to the gateway via usb or serial'
        + ' from the following path:',
        'Are you sure, and have you connected only one node to the gateway?',
        "initialize serial");
}


function pre_flash_wemos() {
    shell_command('You are about to flash a connected Wemos D1 mini with'
        + ' a default ulnoiot firmware. Are you sure, and have you connected '
        + 'exactly one Wemos D1 mini via USB to the gateway?',
        "pre_flash_wemos");
}


function shell_escape() {
    shell_command_in_path("Starting the shell (bash) now. "
        + "Remember to type exit to leave. Initial path will be:",
        null, "bash");
}


function advanced() {
    choice([
        ["Pre-Flash Wemos D1 mini (P)", "P", pre_flash_wemos],
        ["Initialize Serial (I)", "I", initialize_serial], 
        ["Compile (C)", "C", compile], 
        ["Upgrade (U)", "U", upgrade],
        ["Shell Escape (S)", "S", shell_escape],
        ["Back (B,X,ESC)", ["B","X"], terminate]
    ], pre_select=-1, pad_last=1);
}


function menu_default() {
    //term.clear();
    term("\n\n");
    term.bgBrightWhite.black.wrap( '^+ul^rno^-^kiot^: Simple Shell\n' ) ;
    term("\n");
    term.wrap('Use mouse, arrow keys, enter, or shortcut key to select.\n')
    term("\n");

    choice([
        ["Deploy (D)", "D", deploy], 
        ["Initialize (I)", "I", initialize],
        ["Create Node Template (N)", "N", create_node_template],
        ["Advanced (A)", "A", advanced],
    //    "Exit (X,ESC)"
    ], pre_select=0, pad_last=0);
}

menu_default();