module.exports = {
    'D - Deploy': async ({CloudCmd}) => {
         await CloudCmd.TerminalRun.show({
             command: 'iot menu deploy',
             autoClose: true,
         });
        
        await CloudCmd.refresh();
     },
    'A - Adopt': async ({CloudCmd}) => {
         await CloudCmd.TerminalRun.show({
             command: 'iot menu adopt',
             autoClose: true,
         });
        
         await CloudCmd.refresh();
     },
    'C - "Wifi Configuration': async ({CloudCmd}) => {
         await CloudCmd.TerminalRun.show({
             command: 'iot menu system_configuration',
             autoClose: true,
         });
        
         await CloudCmd.refresh();
     },
         
    'N - Create New Node Folder': async ({CloudCmd}) => {
         await CloudCmd.TerminalRun.show({
             command: 'iot menu create_node_template',
             autoClose: true,
         });
        
         await CloudCmd.refresh();
     },
    'V - Advanced': async ({CloudCmd}) => {
         await CloudCmd.TerminalRun.show({
             command: 'iot menu advanced',
             autoClose: true,
         });
        
         await CloudCmd.refresh();
     },

    'M - Full IoTempower User Menu': async ({CloudCmd}) => {
        await CloudCmd.TerminalRun.show({
            command: 'iot menu',
            autoClose: true,
        }); 
        await CloudCmd.refresh();
    },
    'F2 - Rename file': async ({DOM}) => {
        await DOM.renameCurrent();
    }
}
