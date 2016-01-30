Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
        var req = new XMLHttpRequest();
        req.open('GET', 'http://panopticon.ballistaline.com/pebble.php?data=sdfsfsd', true);
    }
);
