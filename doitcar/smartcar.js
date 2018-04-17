var lastMove = 0;
function move(left, right) {
  left = Math.min(1023, Math.max(-1023, left));
  right = Math.min(1023, Math.max(-1023, right));
  var now = Date.now();
  if (lastMove + 200 < now) {
     lastMove = now; 
     var request = new XMLHttpRequest();
     request.open('GET', '/engines/' + Math.round(left) + "," + Math.round(right), true);
	 
	 const requestString = '/engines/' + Math.round(left) + "," + Math.round(right);
	 document.getElementById("dmEvent").innerHTML = requestString;
	 
	 request.addEventListener('load', function(event) {
      if (request.status >= 200 && request.status < 300) {
         console.log(request.responseText);
		 document.getElementById("EPSStatus").innerHTML =request.responseText;
      } else {
         console.warn(request.statusText, request.responseText);
      }
   });
     request.send(null);
  }
}
document.onkeydown = function detectKey(event) {
    var e = event.keyCode;
    if (e==87){ move(600, 600);}
    if (e==83){ move(600, -600);}
    if (e==65){ move(-600, 600);}
    if (e==68){ move(-600, -600);}
}

if (window.DeviceMotionEvent) {
  window.addEventListener('devicemotion', deviceMotionHandler, false);
} else {
  document.getElementById("dmEvent").innerHTML = "Accelerometer not supported."
}
function deviceMotionHandler(eventData) {
  acceleration = eventData.accelerationIncludingGravity;
  var left = 0;
  var right = 0;
  if (Math.abs(acceleration.y) > 1) {
    var speed = acceleration.y * 150;
    left = Math.min(1023, speed + acceleration.x * 75);
    right = Math.min(1023, speed - acceleration.x * 75);
  } else if (Math.abs(acceleration.x) > 0.75) {
    var speed = Math.min(1023, Math.abs(acceleration.x) * 150);
    if (acceleration.x > 0) {
      left = speed;
      right = -speed; 
    } else {
      left = -speed;
      right = speed;
    }
  }
  if (Math.abs(left) > 200 || Math.abs(right) > 200) {
    move(left, right);
  } else {
    move(0, 0);
  }
  var direction = "stop";
  direction = "[" + Math.round(acceleration.x) + "," + Math.round(acceleration.y) + "," + Math.round(acceleration.z) + "]<BR/>" + Math.round(left) + ", " + Math.round(right); 
  document.getElementById("vector").innerHTML =direction;
}