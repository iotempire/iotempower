============
Segundo Node
============

Para esta seção, presumimos que você configurou com sucesso o UlnoIoT
no Raspberry Pi e já consiga acessar as páginas web do sistema nele.
Também como presumimos que você já tenha configurado seu primeiro no
`Primeiro Node <first-node-pt.rst>`_ e pode controlar o LED da
placa microcontroladora através do node-RED web gui.

O objetivo deste tutorial é mostrar como adotar e inicializar um segundo
node e depois conectá-lo ao primeiro sem fio *Over the Air (OTA)*
via Node-RED.

Vamos começar então...

Novo Node
---------

-   Navegue de volta para a sua `IoT system example configuration folder
    <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/>`_.

    Como você pode notar, cada Node tem sua própria pasta de configuração
    no UlnoIoT.

-   Precisamos, portanto, criar outra pasta para o segundo Node.
    Nós podemos rapidamente crie uma pasta usando como modelo o `node-template`.

    Para isso, certifique-se de que você ainda esteja na pasta
    `example system configuration` e ative novamente o menu do usuário clicando neste botão:

    .. image:: /doc/images/user-menu-button.png

    No menu do usuário,
    selecione ``Create New Node Folder`` e depois de verifique se
    o caminho de destino está correto (ele deve ser ``iot-test/``), selecione
    ``Yes, run create_node_template``. Certifique a a criação teve sucesso
    e aperte ``OK`` e você voltará ao painel de arquivos do cloudcmd no navegador.

    Você deve ver lá uma nova pasta chamada `new-node`
    (em baixo da pasta ``node1``, do ``README.rst`` do ``system.conf``).

-   Selecione a nova pasta usando o mouse ou o teclado
    e pressione o botão renomear
    `rename button` na parte de baixo da tela (ou a tecla F2) e renomeie a pasta para ``node2``.

-   Navegue até a pasta ``node2``, você deverá ver agora os seguintes arquivos:
    ``README.rst``, ``key.txt``, ``node.conf`` e ``setup.cpp``.

    Se você não estiver usando um
    *Wemos D1 Mini* como um novo Node, edite o
    ``node.conf`` trocando o nome
    `Wemos D1 Mini` pelo nome da placa que irá usar.
    Se estiver usando *Wemos D1 Mini* não é preciso fazer nada.

-   Certifique-se de abrir outra `ulnoiot documentation web-page </>`_ para
    referência. Abra a `command reference </doc/node_help/commands.rst>`_.

    Nos arquivos, selecione ``setup.cpp``
    e edite-o (com o botão de edição `edit button` 
    na parte inferior da tela  ou a tecla F4).

    Você deve ver um pequeno bloco de comentários explicativos.
    Você deverá agora
    removê-lo completamente deletando
    tudo para ter um arquivo de configuração vazio.

-   Na referência do comando abra o botão `help` e copie de lá o
    exemplo de configuração `example configuration`
    (incluindo ponto e vírgula),
    algo assim: ``input (lower, D2, "depressed", "pressed");``

    Cole o exemplo no editor ``setup.cpp``.

-   Edite-o para  ``input (b1, D3," up "," down ");``

-   Saia do editor (pressione *ok* ou com
    o mouse aperte o *x* no canto superior direito)
    e aceite salvar o arquivo.

-   Como fizemos no tutorial do `Primeiro Node <first-node-pt.rst>`_,
    vamos adotar agora
    node2. (Anexe o botão, ative o modo
    de adoção, conte o padrão de piscadas longas e curtas,
    use o telefone para definir
    credenciais de rede WiFi do Node e dentro da pasta ``Node 2``
    aperte o botão `adopt/initialize` localizado no menu do usuário).

Programando as conexões visualmente no Node-RED
-----------------------------------------------

-   Após a adoção bem-sucedida, abra a página `Node-RED </nodered/>`_.
    Aqui, você
    provavelmente terá que inserir o `username` (*ulnoiot*) e
    `password` (*iotempire*) novamente.

    Você deverá ver agora um programa em Node-RED com cinco nós (`nodes`).

-   Note ​​aqui os dois Nodes de  cor-de-rosa nas laterais, um rotulado
    ``node1/b1``
    e um ``node1/blue/set``.

    Esses são os chamados nodes mqtt (Message Queuing Telemetry Transport).
    Eles são a nossa ponte para a linguagem
    de comunicação da Internet das coisas (IoT).

-   O novo Node que acabamos de configurar já está enviando dados para o tópico
    ``node2/b1``.

-   Portanto vamos criar um novo node de entrada mqtt, arrastando-o do
    painel da esquerda para dentro do fluxo atual.
    Clique duas vezes no node e defina seu tópico para ``node2/b1``.

-   Também crie um node `debug` de saída de (arraste-o do painel para o fluxo)
    e desenhe um fio da porta de saída do node mqtt para a porta de entrada
    do `debug`.

-   Pressione o botão  `deploy` vermelho (no canto superior direito).

-   Sob o botão `deploy` vermelho tem uma pequena coluna de abas. Encontre a
    com o símbolo de um pequeno inseto e ative-a.
    Agora pressione o botão que está  conectado ao seu novo node (node2)
    várias vezes.

    Você deverá ver várias mensagens ``up`` e ``down`` aparecerem
    no log de `debug` no painel a direita.

-   Adicione um novo nó chamado ``change`` (do painel da esquerda)
    e configure-o para substituir `up` por `off` e` down` por `on`.

-   Insira este nó ``change`` no fluxo entre o node de entrada mqtt ``node2``
    para e o node de saída mqtt rotulada ``node1/blue/set``.

-   Implemente apertando o botão `deploy` vermelho e depois pressione o
    botão conectado a placa do ``node2``, observe o LED azul da placa do
    ``node1`` acender
    e apagar.

Parabéns!! Você pode controlar remotamente o seu LED!

Se você ainda tiver tempo, tente usar esse novo botão para *alternar*
o estado do LED. Para isso estude os nós `rbe` e `toggle` no Node-RED.


Topo: `ToC <index-doc.rst>`_ , Anterior: `Primeiro Node <first-node-pt.rst>`_ ,
Próximo: `External Resourses <resources.rst>`_.
`English version of this page is here <second-node.rst>`_.

