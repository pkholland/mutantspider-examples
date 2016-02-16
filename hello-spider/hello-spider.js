document.addEventListener('DOMContentLoaded', function(event) {

  var bd_elm = document.getElementById('black_dot');
  var last_x = 0, last_y = 0;
  var update_waiting = false;
  
  function display() {
    if (!update_waiting) {
      update_waiting = true;
      gen_blackdot(bd_elm.clientWidth, bd_elm.clientHeight, last_x, last_y);
    }
  }
  
  set_bitmap_listener(function(width, height, pixels) {
    var canvas_ctx = bd_elm.getContext('2d');
    var id = canvas_ctx.createImageData(width,height);
    var buff = new Uint8ClampedArray( pixels, 0, width*4*height );
    id.data.set(buff);
    canvas_ctx.putImageData(id,0,0,0,0,width,height);
    update_waiting = false;
  });

  // initial display
  bd_elm.width = window.innerWidth;
  if (bd_elm.width > 800)
    bd_elm.width = 800;
  bd_elm.height = bd_elm.width * .666;
  display();

  var bd_elm = document.getElementById('black_dot');
  var mouse_is_down = false;
  
  bd_elm.onmousedown = function(evt) {
    evt.preventDefault();
    mouse_is_down = true;
    last_x = evt.offsetX;
    last_y = evt.offsetY;
    display();
  }
  
  bd_elm.onmousemove = function(evt) {
    if (mouse_is_down) {
      evt.preventDefault();
      last_x = evt.offsetX;
      last_y = evt.offsetY;
      display();
    }
  }
  
  bd_elm.onmouseup = function(evt) {
    evt.preventDefault();
    mouse_is_down = false;
  }
    
  window.addEventListener('resize',function() {
    bd_elm.width = window.innerWidth;
    if (bd_elm.width > 800)
      bd_elm.width = 800;
    bd_elm.height = bd_elm.width * .666;
    display();
  });

});
