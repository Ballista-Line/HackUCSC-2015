//NOTE: ready is only sent once per run of pebbleparty
Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
    }
);

Pebble.addEventListener('appmessage',
  function(e) {
    var message = JSON.stringify(e.payload);
    console.log('Received message: ' + message);
    var req = new XMLHttpRequest();
    req.open('GET', 'http://panopticon.ballistaline.com/pebble.php?data='+message);
    req.send();
  }
);