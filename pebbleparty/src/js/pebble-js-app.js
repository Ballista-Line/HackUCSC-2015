
var token = null;
//NOTE: ready is only sent once per run of pebbleparty
Pebble.addEventListener("ready",
    function(e) {
        token = Pebble.getWatchToken();
        console.log("Hello world! - Sent from your javascript application.");
        var req = new XMLHttpRequest();
        req.open('GET', 'http://panopticon.ballistaline.com/pebble.php?create=true&id='+token);
        req.send();
    }
);

Pebble.addEventListener('appmessage',
  function(e) {
    var message = JSON.stringify(e.payload);
    console.log('Received message: ' + message);
    var req = new XMLHttpRequest();
    req.open('GET', 'http://panopticon.ballistaline.com/pebble.php?data='+message+'&id='+token);
    req.send();
  }
);