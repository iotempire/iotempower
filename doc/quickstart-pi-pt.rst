Configuração Rápida do Raspberry Pi
===================================

Após `instalar a imagem do UlnIoT no seu cartão SD <image-pi.rst>`_, 
insira o cartão SD novamente no leitor de cartão do seu computador (MacOS,
Windows or Linux) para configurar o roteador WiFi do seus sistema UlnIoT.

Configurando o roteador WiFi no seu Raspberry Pi
------------------------------------------------

- No seu computador, abra a pasta do seu cartão SD.

- Encontre o arquivo chamado ``wifi-setup.txt`` (no Windows a extensão
  ``.txt`` talvez não apareça). 

- Abra o arquivo ``wifi.txt`` e modifique os valores 
  (#iotempire-123456 e iotempire) abaixo do ``Wifi Name`` e ``Password`` 
  por um nome e password de sua preferência. Certifique-se que seu password
  tenha mais de 8 caractéres. 

  .. code-block:: bash

     ### WiFi Name (avoid blanks) ###
     #iotempire-123456
     ### Password (at least 8 characters, max 32, avoid blanks) ###
     iotempire

  Essa mudança irá configurar as credenciais para o roteador WiFi do Raspberry
  Pi. Anote essas credenciais pois irá precisar para logar a rede WiFi do Pi
  desde o seu computador.

- Se você tiver acesso à ethernet (por exemplo, uma porta LAN Ethernet no seu
  roteador), conecte o Pi a esta porta ethernet para que ele possa acessar a
  internet - nenhuma configuração extra é necessária para isso.
  Se você não tiver acesso a uma porta ethernet, vá para a próxima etapa.
  [# f1] _

- Coloque o cartão SD em um Raspberry Pi 3 e ligue-o.

- Agora você deve ver sua rede Wi-Fi UlnoIoT conforme especificado em
  ``Name WiFi``.
  Conecte seu computador (laptop ou desktop) a esta rede WiFi
  (use a senha definida antes em `` Password``).

Acessando os serviços locais no Raspberry Pi
--------------------------------------------

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
  </cloudcmd/fs/home/ulnoiot/iot-test>`_,
  você deve ver agora os arquivos do sistema
  dentro da pasta IoT-test, sendo uma pasta
  ``node1``, um arquivo `` README.rst`` e
  outro `` system.conf``. Por enquanto,
  ignore o segundo painel de arquivos na parte
  direita da tela, primeiro
  vamos nos concentrar no painel ativo a esquerda.

- Confira o conteúdo de README.rst usando o botão view ou visualize
  na menu de contexto (para sair da visualização aperte *Esc* ou o *x*
  na parte superior
  a direita).

- Navegue até a pasta `` node1`` e de uma
  olhada no conteúdo do arquivo
  `` setup.cpp``. Um pouco críptico? Não tenha medo disso,
  ele apenas diz que queremos configurar um dispositivo com
  um botão e tornar acessível o led azul da sua placa
  microcontroladora, sai da visualização de conteúdo do
  ``setup.cpp`` novamente (apertando *Esc* ou ou o *x*
  na parte superior a direita).

.. Se você tiver problemas em seguir este tutorial, assista aos vídeos tutoriais no
    Youtube. TODO: fornecer página com links!

Agora você pode continuar com seu `Primeiro Node IoT <first-node-pt.rst>`_.

.. rubric:: Notas de rodapé

.. [#f1] Se seu Raspberry Pi não estiver conectado à internet, um computador 
   logado em sua Rede Wi-Fi pode mudar de volta para outra rede Wi-Fi
   automaticamente por preferir redes com conexão com à internet.
   Certifique-se de verificar se você está conectado à rede de Pi
   regularmente.

Topo: `ToC <index-doc.rst>`_, Anterior: `Installation <installation.rst>`_,
Próximo: `Primeiro Node IoT <first-node-pt.rst>`_.
`English version of this page is here <quickstart-pi.rst>`_.
