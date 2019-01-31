var term = require( 'terminal-kit' ).terminal ;
term.grabInput( { mouse: 'button' } ) ;
term.hideCursor(true);

//term.clear();
term("\n\n");
term.bgBrightWhite.black( '^+ul^rno^-^kiot^: Simple Shell\n\n' ) ;
term('Use mouse, arrow keys, enter, or shortcut key to select.\n')

var items = [
    "Deploy (D)", 
    "Initialize (I)",
    "Upgrade (U)",
    "Advanced (A)",
    "Exit (X,ESC)"
];

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
    if(items[i].length > longest) longest = items[i].length;
}
longest += 6; // Add button inside padding

// create formatted_items
var formatted_items = [];
for(var i=0; i<items.length; i++) {
    var padlength = longest - items[i].length;
    var fstr = 
            " ".repeat(button_padding) + top_left + top_bar.repeat(longest) 
                + top_right + "\n" +
            " ".repeat(button_padding) + bar_left + " ".repeat(Math.floor(padlength/2)) + items[i] 
                + " ".repeat(Math.floor((padlength+1)/2)) + bar_right + "\n" +
            " ".repeat(button_padding) + bottom_left + bottom_bar.repeat(longest) 
                + bottom_right + " ".repeat(button_padding);
    formatted_items.push(fstr);
}
// add line before last
formatted_items[items.length-1] = "\n" + formatted_items[items.length-1]

term.singleColumnMenu( formatted_items , 
    { 
        leftPadding: "",
        selectedLeftPadding: "",
        cancelable: true,
        selectedIndex: 4,
        itemMaxWidth: 2*(button_padding+1)+longest,
        extraLines: 2
    }, 
    function( error , response ) {
        term( '\n' ).eraseLineAfter.green(
            "#%s selected: %s\n" ,
            response.selectedIndex ,
            items[response.selectedIndex]
        );
        term.hideCursor(false);
        process.exit();
    });
