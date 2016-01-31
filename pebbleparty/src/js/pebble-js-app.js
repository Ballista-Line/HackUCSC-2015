
var token = null;
var address = "";
//NOTE: ready is only sent once per run of pebbleparty
Pebble.addEventListener("ready",
    function(e) {
        token = Pebble.getWatchToken();
        console.log("Hello world! - Sent from your javascript application. Token: "+token);
    }
);

Pebble.addEventListener('appmessage',
  function(e) {
    if(e.payload.ADDRESS!=null){
      address = e.payload.ADDRESS;
      console.log('Using address: '+address)
      var req = new XMLHttpRequest();
      req.open('GET', address+'?id='+token);


      req.onreadystatechange = function() {
        if (req.readyState === 4) { //Done

          if (req.status === 200) { //Worked
            var dict = {'MESSAGE':'Connected', 'CONNECTED':1};
            console.log("Connected to server!");
          } else {
            var dict = {'MESSAGE':'Failed to connect', 'CONNECTED':0};
            console.log("Failed to connect to server")
          }

          //Tell pebble if we were able to connect to the server
          Pebble.sendAppMessage(dict, function(e) {
            console.log('Sent message.');
          }, function(e) {
            console.log('Send failed!');
          });

        }
      }


      req.send();


      //Tell pebble if we were able to connect to the server


    }else{
      var message = JSON.stringify(e.payload);
      console.log('Received message: ' + message);
      var req = new XMLHttpRequest();
      req.open('GET', address+'?data='+message+'&id='+token);
      req.send();
    }
  }
);