var Clay = require('pebble-clay');
var clayConfig = require('./config');
var messageKeys = require('message_keys');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }
  
  var dict = clay.getSettings(e.response);
  dict[messageKeys.Language] = parseInt(dict[messageKeys.Language]);
  Pebble.sendAppMessage(dict);
});