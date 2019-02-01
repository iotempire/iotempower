var term = require( 'terminal-kit' ).terminal ;

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

function deploy() {
    term('\n\nYou want to deploy in the following path:\n') ;
    var d = process.env.ACTIVE_DIR;
    if(!d) {
        d = process.cwd();
    }
    // Strip home directory
    var home = process.env.HOME;
    if(d.startsWith(home)) d = d.slice(home.length+1);
    term(d, "\n\nAre you sure?\n");
    
    choice([
        ["Yes, deploy (Y)", "Y", terminate],
        ["No, go back (N)", "N", terminate]
    ], pre_select=-1);
}

function initialize() {
}

function upgrade() {
}

function advanced() {
    choice([
        ["Initialize Serial (I)", "I", terminate], 
        ["PreFlash Wemos (P)", "P", terminate],
        ["Shell Escape (S)", "S", terminate],
        ["Back (B,X,ESC)", ["B","X"], terminate]
    ], pre_select=-1, pad_last=1);
}


function menu_default() {
    //term.clear();
    term("\n\n");
    term.bgBrightWhite.black( '^+ul^rno^-^kiot^: Simple Shell\n' ) ;
    term("\n");
    term('Use mouse, arrow keys, enter, or shortcut key to select.\n')
    term("\n");

    choice([
        ["Deploy (D)", "D", deploy], 
        ["Initialize (I)", "I", initialize],
        ["Upgrade (U)", "U", upgrade],
        ["Advanced (A)", "A", advanced],
    //    "Exit (X,ESC)"
    ], pre_select=0, pad_last=0);
}

menu_default();