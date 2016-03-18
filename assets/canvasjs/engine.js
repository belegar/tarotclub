// CopyRight (c) 2013 John Robinson - http://www.storminthecastle.com

var Engine = function(o) {

  var self = this;
  
    // Setup defaults
  self.type = "2d";
  self.animate = true;
  self.fullscreen = false;
  self.keysDown = {};
  self.userData = {};

  if (o) {
   for (var p in o) {
     if (!o.hasOwnProperty(p)) continue;
     self[p] = o[p];
   }
  }
  
  self.canvas = document.getElementById("canvas");
  if (self.canvas)
    self.ctx = self.canvas.getContext(self.type);

  if (!self.canvas || !self.ctx && self.onFailure) {
    self.onFailure();
    return;
  }

  self.width = self.canvas.width;
  self.height = self.canvas.height;

  var getTick = Date.now?Date.now: function () {
      return new Date().getTime();
    }

  self.updateMouse = function(x, y) {

    var self = this;
    	
    var obj = self.canvas;
    while (obj) {
      y -= obj.offsetTop;
      x -= obj.offsetLeft;
      obj = obj.offsetParent;
    }

    self.prevMouseX = self.mouseX;
    self.prevMouseY = self.mouseY;

    self.mouseX = x;
    self.mouseY = y;
  }

  document.addEventListener("mousedown", function(e) {
    //  if (e.target == self.canvas) {
        self.updateMouse(e.pageX, e.pageY);
        if (self.onMouseDown)
          self.onMouseDown(self);
    //    self.dragging = true;
    //  }
    }, false);

  document.addEventListener("mouseup", function(e) {
      if (self.dragging) {
      self.updateMouse(e.pageX, e.pageY);
      if (self.onMouseUp)
        self.onMouseUp(self);
      self.dragging = false;
      }
    }, false);
    
  document.addEventListener("mousemove", function(e) {
      self.updateMouse(e.pageX, e.pageY);
      if (self.mouseX != self.prevMouseX || self.mouseY != self.prevMouseY) {
        if (self.dragging && self.onMouseDrag)
          self.onMouseDrag(self);
        else if (self.onMouseMove)// && e.target == self.canvas)
          self.onMouseMove(self);
      }
    }, false);


  document.addEventListener("touchstart", function(e) {
  //    if (e.target == self.canvas) {
        self.updateMouse(e.pageX, e.pageY);
        if (self.onMouseDown)
          self.onMouseDown(self);
        self.dragging = true;
  //    }
    }, false);
    
  document.addEventListener("touchend", function(e) {
      if (self.onMouseUp)
        self.onMouseUp(self);
      self.dragging = false;
    }, false);
          
  document.addEventListener("touchmove", function(e) {
      if (self.dragging)
      {
        self.updateMouse(e.pageX, e.pageY);
        self.onMouseDrag(self);
        e.preventDefault();
      }
  }, false);
    
  document.addEventListener("click", function(e) {
      self.updateMouse(e.pageX, e.pageY);
      if (self.onMouseClick)
        self.onMouseClick(self);
    }, false);
          
  document.addEventListener("keydown", function(e) {
      self.keyCode = e.keyCode;
      self.key =  String.fromCharCode(self.keyCode);
      self.keysDown[self.keyCode] = true;
      if (self.onKeyDown) self.onKeyDown(self)
    }, false);
    
  document.addEventListener("keyup", function(e) {
      self.keyCode = e.keyCode;
      self.key =  String.fromCharCode(self.keyCode);
      delete self.keysDown[self.keyCode];
      if (self.onKeyUp) self.onKeyUp(self)
    }, false);

  document.addEventListener("keypress", function(e) {
      self.keyCode = e.keyCode;
      self.key =  String.fromCharCode(self.keyCode);
      if (self.onKeyPress) self.onKeyPress(self)
    }, false);

   var sizeCanvas = (function() {
    if (self.fullscreen) {

      // This performs better than listening for resize events
      var w = window.innerWidth;
      var h = window.innerHeight;

      if (self.canvas.width != w) {
        self.canvas.width = w;
      }
      if (self.canvas.height != h){
        self.canvas.height = h;
      }
    }  
  });
  
  var fpsCounter = 0;
  var fpsStartTime = getTick();
  
  self.update = function() {

    sizeCanvas();
    var tick = getTick();

    if (self.onDraw) {
      self.onDraw(self, tick);
    }

    if (self.animate) {
      if (fpsCounter++ > 60) {
        self.fps = fpsCounter / (tick-fpsStartTime) * 1000;
        if (self.onFrameRate)
          self.onFrameRate(self);
        fpsCounter = 0;
        fpsStartTime = tick;
      }
      window.requestAnimationFrame(self.update);
    }
  };

  window.requestAnimationFrame(self.update);
};
