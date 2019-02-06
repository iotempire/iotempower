=============
Primeiro Node
=============

Para essa seção presumimos que você já tenha configurado com sucesso 
o UlnoIoT em um Raspberry Pi e já consiga acessar as páginas web nele.

Vamos agora configurar nosso primeiro node IoT (se você quer saber
o que é um *node*, acesse o `architecture chapter <architecture.rst>`_).

O UlnoIoT suporta entre outras as placas microcontroladoras listas a baixo
(para a lista completa de placas suportadas acessem
`hardware chapter <hardware.rst>`_):

- Wemos D1 Mini Mini
- NodeMCU
- Espresso Lite V2

Você precisará de um *smart phone* ou *computador* para configurar
este primeiro node.

Se você usar um Wemos D1 Mini como seu primeiro node (esse é o default), 
nenhuma ajuste é necessário. Se você está usando um NodeMCU ou outra placas
você precisará mudar a configuração (no arquivo ``node.cong`` localizado na 
pasta do seu node) para NodeMCU ou a respectiva placa.

Se você estiver seguindo esse tutorial em uma aula de UlnoIoT sua placa já 
estará pré-instalada com o firmware do UlnoIoT.
Caso o contrário você terá que seguir as instruções dessa página
`First Flash page <pre-flash.rst>`_ no seu Raspberry Pi 
para saber como instalar pela primeira vez o UlnIoT em uma placa.

Configurando as credenciais do WiFi no seu primeiro node
--------------------------------------------------------

- Note que essa instruções são para reconfigurar os dados WiFi em nodes
  onde o firmware do UlnIoT já pré-instalado e por isso podem ser 
  configurados sem fio ( com *over the air - OTA* - os nodes usados em sala
  de aula já estão configurados para tal). 
  Se o seu node nunca foi instalado com UlnIoT antes siga as instrução desse
  tutorial aqui `First Flash page <pre-flash.rst>`_ 
  para a primeira instalação.

- Agora vamos começar. Conecte um botão, no caso do Wemos D1 mini, ao pino
  D3 (outras placas costumam ser no pino 0) e o Ground (terra) ou conecte 
  o shield com botão em cima da placa. Esse botão será usado para deixarmos
  a node microcontroladora em *adoption mode* (modo de adoção).

- Ligue seu node na energia através de uma bateria ou um carregador micro
  usb. Espere até que o LED do node comece a piscar e aperte o botão que
  configuramos no passo anterior 2 ou 3 vezes durante os primeiros 5 segundos.
  O LED deverá piscar em um padrão de pulsos lentos e rápidos. 
  Se o LED estiver aceso ou apagado continuamente significa que o processo
  não funcionou. Desconecte o node de energia e reconecte e tente
  novamente este processo. 

- Se você executou o passo anterior corretamente o node deverá está agora
  no modo de adoção e o LED da node deverá estar piscando em um padrão 
  único com se fosse um código Morse: Algumas piscadas longas e outras 
  curtas, por exemplo 3 piscada longa e 2 curtas.
  Preste atenção neste padrão e conte quantas piscadas longas e curtas ele 
  tem pois esse numero será usado para identificar a rede WiFi da seu node
  no próximo passo. 

- Agora use seu smart phone ou tablet (pode ser um computador também) e vá 
  até o menu de configuração da rede WiFi. Seu node deverá aparecer na lista
  de redes WiFi e terá um nome parecido com ``ulnoiot-ab-mn``. ``ab``
  representam um identificar único e ``mn`` representa justamente o número de
  piscadas longas e curtas respectivamente referente ao padrão do seu node.
  Por exemplo ``uiot-node-ab-12`` seriam 1 piscada longa e 2 curtas.

Em sala de aula poderemos ter muitas redes WiFi com nomes parecidos. 
Certifique-se que o padrão da rede que irá escolher corresponda ao 
padrão de piscadas do seu node (se existe outras redes com o mesmo padrão
escolha a rede com o sinal mais forte das duas). 

- Clique na rede certa para conectar com o WiFi do seu node.

- Se aparecer uma mensagem de alerta dizendo 
  *Internet pode não está disponível* não tem problema. Click *OK*.

- Você deverá ser levado automaticamente a uma página web (parecida com 
  aquelas quando conectamos ao WiFi de aeroportos ou alguns cafés).
  Se esta página não aparecer automaticamente abra um navegador
  (chrome por exemplo) e digite o url http://192.168.4.1 na caixa de endereço

- Clique no botão ``Configure WiFi``.

- Agora prencha o nome e o password do WiFi do seu Raspberry Pi
  (aquele que criamos quando configuramos o UlnoIoT no Raspberry Pi
  no tutorial `Tutorial de configuração do Pi <quickstart-pi-pt.rst>`_)
  com o ``WiFi Name`` e ``Password`` e clique no botão *save*. 

- Se você executou o passo anterior corretamente, o seu node deverá
  reiniciar automaticamente e o LED deverá acender continuamente.
  Se o LED começar a piscar em um padrão único novamente de piscadas
  longas e curtas significa que o passo anterior falhou e você terá que 
  repeti-lo. 

- Agora o seu node está pronto para ser adotado pelo sistema UlnoIoT 
  no Raspberry Pi. 

Adoção do Node pelo sistema UlnoIoT no Raspberry Pi
---------------------------------------------------

- Se você ainda não conectou seu computador (laptop ou desktop) à rede WiFi
  do seu Raspberry Pi (como ensinamos no `Tutorial de configuração do Pi 
  <quickstart-pi-pt.rst>`_) faça isso agora. 

- Conecte ao UlnoIoT através de um navegador web.

- No seu navegador acesse o endereço https://ulnoiotgw 
  (ou https://ulnoiotgw.local se os dois não funcionarem 
  tente https://192.168.12.1).

Se não aceitou a exceção de segurança anteriormente aceite-a agora
para gerar o certificado de segurança local. 

- Se for pedido um nome de usuário e password, use ``ulnoiot`` como nome e
  ``iotempire`` como password.

- Você deverá ver agora a home page do seu UlnIoT local. 

- Abra o link `IoT system example configuration folder
  </cloudcmd/fs/home/ulnoiot/iot-test>`_, você deve ver agora os arquivos do sistema
  dentro da pasta IoT-test, sendo uma pasta `` node1``, um arquivo `` README.rst`` e
  outro `` system.conf``. Por enquanto, ignore o segundo painel de arquivos na parte
  direita da tela, primeiro vamos nos concentrar no painel ativo a esquerda.

- Você deverá ver uma pasta chamada `node1
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/node1/>`_.
  Entre nesta pasta.

- Quando estiver dentro da pasta `node1
  <https://ulnoiotgw.local/cloudcmd/fs/home/ulnoiot/iot-test/node1/>`_
  clique no botão ``=>`` localizado no canto inferior direito da sua tela.

- Você deverá está vendo agora um menu com alguns botões. 

- Clique no botão chamado ``Adopt/Initialize`` e verifique novamente se você 
  está na pasta``node1``. Agora aperte no botão ``Yes, run initialize`` para 
  começar a inicialização do node.

- Espere até que o processo termine e certifique-se de que ele foi concluído 
  com sucesso. Você deverá ver uma mensagem dizendo *deploy successfully done.*
  and *Done initializing.*
  Se a mensagem diz *Initializing not successful, check errors above.* 
  significa que processo falhou. Neste caso, por favor acesse o arquivo
  `troubleshooting <troubleshooting.rst>`_ para mais informações.

Parabéns! Seu node agora está conectado ao UlnIoT e o LED da placa pode ser
controlado usando o botão desse programa Node-RED 
`<https://ulnoiotgw/nodered/ui/#/1>`_

Topo: `ToC <index-doc.rst>`_, Anterior: `Installation <installation.rst>`_,
Próximo: `External Resources <resources.rst>`_.
