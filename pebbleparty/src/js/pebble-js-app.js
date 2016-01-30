
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
    if(e.payload.ADDRESS!=""){
      address = e.payload.ADDRESS;
      console.log('Using address: '+address)
      var req = new XMLHttpRequest();
      req.open('GET', address+'?create=true&id='+token);
      req.send();
    }else{
      var message = JSON.stringify(e.payload);
      console.log('Received message: ' + message);
      var req = new XMLHttpRequest();
      req.open('GET', address+'?data='+message+'&id='+token);
      req.send();
    }
  }
);