
var myAPIKey='057f3b26d03c80bdf01c5370214526ac';

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getCcyPolo() {
  // Construct URL
  var url = 'https://poloniex.com/public?command=returnTicker';

  // Send request to Poloneix
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      var usdt_btc_last = floatFormat(json.USDT_BTC.last,2);
      console.log("USDT_BTC is " + usdt_btc_last);
      // Conditions
      var usdt_btc_change = parseFloat(json.USDT_BTC.percentChange)*100;
      if(usdt_btc_change>=100){
        usdt_btc_change=floatFormat(usdt_btc_change, 1);
      }else{
        usdt_btc_change=floatFormat(usdt_btc_change, 2);
      }      
      usdt_btc_change =String(usdt_btc_change);
      console.log("percentChange are " + usdt_btc_change);
			var usdt_btc_volume =  volFormat(json.USDT_BTC.baseVolume);

      // Assemble dictionary using our keys
      var dictionary = {
        "Usdt_btc_last": usdt_btc_last,
        "Usdt_btc_change": usdt_btc_change,
				"Usdt_btc_volume": usdt_btc_volume
      };
			

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Polo info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending Polo info to Pebble!");
        }
      );
    }
  );
}

function getCcyYahoo() {
  // Construct URL
  var url = 'https://query.yahooapis.com/v1/public/yql?q=';
		url+= 'select%20*%20from%20yahoo.finance.xchange%20where%20pair%20in%20(%22USDJPY,BTCUSD,EURJPY%22)';
		url+= '&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys';

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      // Assemble dictionary using our keys
      var dictionary = {
				"USD_JPY": floatFormat(json.query.results.rate[0].Rate, 2),
				"BTC_USD": floatFormat(json.query.results.rate[1].Rate, 2)
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }
  );
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    // getWeather();
		getCcyPolo();
		getCcyYahoo();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    // getWeather();
		getCcyPolo();
  }
);


function zeroFill(num, fill) {
  var padd = "0000000000";
  return (padd + num).slice(-fill);
}
function floatFormat(number, n) {
  var _number = parseFloat(number);
  var _pow = Math.pow(10, n);
  return (Math.round(_number * _pow) / _pow).toFixed(n);
}

function volFormat(number){
  var _number = parseInt(number);
  if(_number>=1000*1000){
    _number=floatFormat(_number/(1000*1000),1);
    _number= _number + 'M';
  }else if(_number>=1000){
    _number=floatFormat(_number/(1000),1);
    _number= _number + 'K';
  }else{
    _number=floatFormat(_number/(1),1);
    _number=  String(_number) ;
  }
  return _number;
}