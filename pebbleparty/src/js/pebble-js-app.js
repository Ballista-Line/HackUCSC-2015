//NOTE: ready is only sent once per run of pebbleparty
Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
        var req = new XMLHttpRequest();
        req.open('GET', 'http://panopticon.ballistaline.com/pebble.php?data=From the pebble');
        req.send();
    }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload));
  }
);