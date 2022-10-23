const char indexHTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Music Box</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@300;500&display=swap" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/vanillatoasts@1.3.0/vanillatoasts.js"></script>
    <link href="https://cdn.jsdelivr.net/npm/vanillatoasts@1.3.0/vanillatoasts.css" rel="stylesheet"></script>
    <style>
      html,body{
        font-family: 'Space Grotesk', sans-serif;
      }
      body{
        color: #111;
        font-weight: 300;
        background-image: linear-gradient(
            45deg,
            hsl(0deg 100% 9%) 0%,
            hsl(352deg 97% 13%) 8%,
            hsl(349deg 100% 17%) 18%,
            hsl(349deg 100% 21%) 30%,
            hsl(350deg 100% 26%) 45%,
            hsl(351deg 100% 30%) 62%,
            hsl(353deg 100% 35%) 76%,
            hsl(355deg 100% 39%) 87%,
            hsl(357deg 100% 44%) 94%,
            hsl(0deg 100% 48%) 100%
        );
        height: 100vh;
      }
      *{
        box-sizing: border-box;
      }
      strong, b{
        font-weight: 500;
      }
      h1{
        font-size: 28px;
        margin: 0 0 16px 0;
        font-weight: 300;
      }
      h2{
        font-size: 16px;
        margin: 0 0 16px 0;
        font-weight: 500;
      }
      .container{
        position: relative;
        height: 70vh;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        transition: all 0.8s;
      }
      .container--mode_write{
        height: 100vh;
      }
      .cube_container{
        width: 400px;
        height: 400px;
        perspective: 1000px;
        perspective-origin: 50% 50%;


      }

      .cube {
        transform-style: preserve-3d;
        transition: all 0.8s;
        width: 400px;
        height: 400px;
        position: relative;
        transform: translateZ(-200px);
      }


      .cube__face {
        position: absolute;
        width: 400px;
        height: 400px;
        padding: 40px;
        transition: all 0.8s;
        background-color: white;
      }

        .cube__face--front  { transform: rotateY(  0deg) translateZ(200px); }
        .cube__face--right  { transform: rotateY( 90deg) translateZ(200px); }
        .cube__face--back   { transform: rotateY(180deg) translateZ(200px); }
        .cube__face--left   { transform: rotateY(-90deg) translateZ(200px); }
        .cube__face--top    { transform: rotateX( 90deg) translateZ(200px); }
        .cube__face--bottom { transform: rotateX(-90deg) translateZ(200px) ; }
        .cube__face--bottom > div{ transform: rotate(-45deg); }

      .cube.show-front,
      .container--mode_read .cube  { transform: translateZ(-200px) rotateY(   0deg); }
      .cube.show-right,
      .container--mode_form .cube  { transform: translateZ(-200px) rotateY( -90deg); }
      .cube.show-back { transform: translateZ(-200px) rotateY(-180deg); }
      .cube.show-left   { transform: translateZ(-200px) rotateY(  90deg); }
      .cube.show-top    { transform: translateZ(-200px) rotateX( -90deg); }
      .cube.show-bottom { transform: translateZ(-200px) rotateX(  90deg); }

      .container--mode_write .cube {
        transform: translateZ(-500px) translateY(100px) rotateX(-30deg) rotateY(-45deg) rotateZ(0deg);

      }

      .cube__face .content__mode_write{
        position: relative;
        width: 100%;
        height: 100%;
        display: flex;
        align-items: center;
        justify-content: center;
      }
      @keyframes reccard {
        0% { opacity: 0; transform: rotateX( 90deg) translateZ(600px); }
        20% { opacity: 1; }
        60% { transform: rotateX( 90deg) translateZ(230px); }
        95% { opacity: 1; }
        100% { opacity: 0; }
      }
      .rec_card{
        display: none;
        position: relative;
        width: 100%;
        height: 100%;

        transform: rotateX( 90deg) translateZ(230px);
        animation: reccard 3s 0 ease-out;
        animation-iteration-count: infinite;
      }
      .rec_card::before{
          content: '';
          display: block;
           position: absolute;
          inset: 20%;
           background-color: rgb(220,220,220);
           border-radius: 5%;
        }
      *,
      *::before,
      *::after {
        box-sizing: border-box;
      }
      select {
        appearance: none;
        background-color: transparent;
        border: none;
        padding: 0 1em 0 0;
        margin: 0;
        width: 100%;
        font-family: inherit;
        font-size: inherit;
        cursor: inherit;
        line-height: inherit;
        }
      select::-ms-expand {
        display: none;
      }
      label{
        display: block;
        padding: 16px 0 4px 0;
      }
      .select,
      input[type="text"] {
        width: 100%;
        padding: 8px 16px;
        background-color: white;
        border: 1px solid rgba(0,0,0,0.2);
        box-shadow: 1px 1px 2px rgba(0,0,0,0.4);
      }
      select,
      input[type="text"]:focus {
          outline: none;
      }
      input[type="text"]:focus {
        border-color: rgba(0,0,0,0.8);
      }
      button{
        padding: 8px 16px;
        background-color: white;
        border: 1px solid rgba(0,0,0,0.2);
        box-shadow: 1px 1px 2px rgba(0,0,0,0.4);
        cursor: pointer;
      }


      .content__mode_form,
      .content__mode_read,
      .content__mode_write{
        transition: all 0.8s;
        opacity: 0;
      }
      .container--mode_read .content__mode_read{
        opacity: 1;
      }
      .container--mode_form .content__mode_form{
        opacity: 1;
      }
      .container--mode_write .content__mode_write{
        opacity: 1;

      }
      .container--mode_write .rec_card{
        display: block;
      }
      .container--mode_write .cube__face--front{
        background-color: rgb(240,240,240);
      }
      .container--mode_write .cube__face--right{
        background-color: rgb(210,210,210);
      }


    </style>

  </head>
  <body>
    <div id="container" class="container container--mode_read">
      <div class="cube_container">

        <div class="cube">
          <div class="cube__face cube__face--front">
            <div class="content__mode_read">
              <h1>Music Box</h1>
              <button onclick="javascript:setModeForm()">Generate new card...</button>
            </div>
          </div>
          <div class="cube__face cube__face--back">

          </div>
          <div class="cube__face cube__face--right">
            <div class="content__mode_form">
              <p>
                <h2>Fill data of OwnTone Library</h2>
                <label for="mediatype">Media:</label>

                <div class="select">
                  <select name="mediatype" id="mediatype">
                    <option value="album">Album</option>
                    <option value="playlist">Playlist</option>
                    <option value="radio">Radio station</option>
                  </select>
                </div>
                <label for="itemid">Id (in OwnTone url):</label>
                <input type="text" name="itemid" id="itemid" />
                <br />
                <br />
                <button
                    onclick="javascript:sendNFCDataToBox()"
                >
                🔴 Start writing process
                </button>
              </p>
            </div>
          </div>
          <div class="cube__face cube__face--left"></div>
          <div class="cube__face cube__face--top"></div>
          <div class="cube__face cube__face--bottom"></div>
          <div class="rec_card" />
        </div>

      </div>
    </div>
    <script>
      function setModeRead(){
        document.getElementById("container").classList.add("container--mode_read")
        document.getElementById("container").classList.remove("container--mode_write")
        document.getElementById("container").classList.remove("container--mode_form")
      }
      function setModeForm(){
        document.getElementById("container").classList.remove("container--mode_read")
        document.getElementById("container").classList.remove("container--mode_write")
        document.getElementById("container").classList.add("container--mode_form")
      }
      function setModeWrite(){
        document.getElementById("container").classList.remove("container--mode_read")
        document.getElementById("container").classList.remove("container--mode_form")
        document.getElementById("container").classList.add("container--mode_write")
      }

      var webSocket;
      var wsUrl = 'ws://'+window.location.hostname+':81'
      //var wsUrl = 'ws://musicbox.local:81'
      function init(){
        console.log("init();")

        webSocket = new WebSocket(wsUrl)
        webSocket.addEventListener("open", function(event){
          console.log("Connected")
          console.log(event)
          webSocket.send('{"action":"getstatus"}')
          checkHash();
        })

        webSocket.addEventListener("close", function(){
          //alert("Connection is closed.\nPlease, reload the page.")
        })
        webSocket.addEventListener("error", function(){
          //alert("Connection is broken.\nPlease, reload the page.")
        })

        webSocket.onmessage = function (event) {
          processWSCommand(event.data);
        }

        function checkHash(){
          var hash = window.location.hash.substr(1)
          if(hash){
            document.getElementById('mediatype').value = 'album'
            document.getElementById('itemid').value = hash
            sendNFCDataToBox()
          }          
        }
        window.addEventListener('hashchange', () => {
          checkHash()
        }, false);
        
      }

      function sendNFCDataToBox(){
        var wsMessageObject = {
          action: 'setmodewrite',
          mediatype: document.getElementById('mediatype').value,
          itemid: document.getElementById('itemid').value
        }
        var wsMessageString = JSON.stringify(wsMessageObject)
        webSocket.send(wsMessageString)
      }

      function processWSCommand(data){
        var jsonData = JSON.parse(data)
        var action = jsonData.action;
        switch(action){
          case "mode_change":
            switch(jsonData.mode){
              case "read":
                setModeRead();
                break;
              case "write":
                setModeWrite();
                break;
            }
          case "msg":
            console.log(jsonData.msg)
            //msgInfo(jsonData.msg)
          break;

          case "msgError":
            console.log(jsonData.msg)
            msgError(jsonData.msg)
          break;

        }
      }
      function msgError(msg){
        VanillaToasts.create({
          // notification title
          title: 'Error',
          // notification message
          text: msg,
          // success, info, warning, error   / optional parameter
          type: 'error', 
          // path to notification icon
          //icon: '/img/alert-icon.jpg',
          // topRight, topLeft, topCenter, bottomRight, bottomLeft, bottomCenter
          positionClass: 'bottomLeft',
          // auto dismiss after 5000ms
          timeout: 5000,
          // hide any other toasts, keeping only the latest visible
          //single: true,
          // executed when toast is clicked
          //callback: function() { ... } 
        });
      }
      function msgInfo(msg){
        VanillaToasts.create({
          // notification title
          title: 'Info',
          // notification message
          text: msg,
          // success, info, warning, error   / optional parameter
          type: 'info', 
          // path to notification icon
          //icon: '/img/alert-icon.jpg',
          // topRight, topLeft, topCenter, bottomRight, bottomLeft, bottomCenter
          positionClass: 'bottomLeft',
          // auto dismiss after 5000ms
          timeout: 10000,
          // hide any other toasts, keeping only the latest visible
          //single: true,
          // executed when toast is clicked
          callback: function() { 
            var wsMessageObject = {
              action: 'restart',
            }
            var wsMessageString = JSON.stringify(wsMessageObject)
            webSocket.send(wsMessageString)
           } 
        });
      }
      window.onload = function () {
        init()
      }

    </script>
  </body>
</html>)rawliteral";
