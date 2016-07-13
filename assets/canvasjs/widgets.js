var Thing = function(o) {
  var self = this;

  self.animated = {x: 0.0, y: 0.0, r: 0, a: 1.0, s: 1.0};


  if (o) {
   for (var p in o) {
     if (!o.hasOwnProperty(p)) continue;
     self[p] = o[p];
   }
  }

  self.update = function(g, t) {
    if (!self.startTime) {
      self.startTime = t;
      if (self.start) {
        var s = self.start;
        for (var p in s) {
          if (!s.hasOwnProperty(p)) continue;
          self.animated[p] = s[p];
        }
      }
    }
    if (self.animation) {
      for (var i = 0; i < self.animation.length; i++) {
        var a = self.animation[i];
        var t0 = self.startTime + a.t0*1000;
        var t1 = self.startTime + a.t1*1000;
        if (t0 <= t && t <= t1) {
          self.animated[a.k] = a.v0 + (((t-t0)/(t1-t0)) * (a.v1-a.v0));
        }
        else if (t1 < t) self.animated[a.k] = a.v1;
      }
    }

    if (self.onDraw)
    {
      var c = g.ctx;
      var a = self.animated;
      var r = a.r * Math.PI / 180;
      c.save();
      c.globalAlpha = a.a; 
      c.translate(a.x, a.y);
	  c.scale(a.s, a.s);
      c.rotate(r);
      self.onDraw(g, t);
      c.restore();
    }

    // The keep option makes the object persistant
    if (self.keep) {
        return self.keep;
    }
    return (self.startTime + self.duration*1000 >= t);
  }

   self.handleEvent = function(g, x, y) {
      if (self.onMouseMove) {
          self.onMouseMove(g, x, y);
      }

  }

};

var Things = function() {

  var self = this;
  var things = [];

  self.add = function(t) {
    things.splice(0, 0, t);
  }

  self.update = function(g, t) {
    for (var i = things.length-1; i >= 0; i--) {
      var thing = things[i];
      if (!thing.update(g, t))
        things.splice(i, 1);
    }
  }

  self.handleEvent = function(g, x, y) {
      for (var i = things.length-1; i >= 0; i--) {
        things[i].handleEvent(g, x, y);
      }
  }


};


/*
var Person = Class.extend({
  init: function(isDancing){
    this.dancing = isDancing;
  },
  dance: function(){
    return this.dancing;
  }
});
*/
var Widget = Class.extend({

    init: function(o) {
        this.animated = {x: 0, y: 0, r: 0, a: 1.0, s: 1.0};
		this.keep = true;
/*
        if (o) {
         for (var p in o) {
           if (!o.hasOwnProperty(p)) continue;
           this[p] = o[p];
         }
        }
        */

    },

  update: function(g, t) {
    if (!this.startTime) {
      this.startTime = t;
      if (this.start) {
        var s = this.start;
        for (var p in s) {
          if (!s.hasOwnProperty(p)) continue;
          this.animated[p] = s[p];
        }
      }
    }
    if (this.animation) {
      for (var i = 0; i < this.animation.length; i++) {
        var a = this.animation[i];
        var t0 = this.startTime + a.t0*1000;
        var t1 = this.startTime + a.t1*1000;
        if (t0 <= t && t <= t1) {
          this.animated[a.k] = a.v0 + (((t-t0)/(t1-t0)) * (a.v1-a.v0));
        }
        else if (t1 < t) this.animated[a.k] = a.v1;
      }
    }

    if (this.onDraw)
    {
      var c = g.ctx;
      var anim = this.animated;
      var r = anim.r * Math.PI / 180;
      c.save();
      c.globalAlpha = anim.a;
      c.translate(anim.x, anim.y);
      c.scale(anim.s, anim.s);
      c.rotate(r);
      this.onDraw(g, t);
      c.restore();
    }

    // The keep option makes the object persistant on the canvas
    if (this.keep) {
        return this.keep;
    }
    return (this.startTime + this.duration*1000 >= t);
  },

  mouseMoveEvent: function(g, x, y) {
      if (this.onMouseMove) {
          this.onMouseMove(g, x, y);
      }
  }

});


var DisplayList = function() {

  var self = this;
  var widgets = [];

  self.add = function(t) {
    widgets.splice(0, 0, t);
  }

  self.update = function(g, t) {
    for (var i = widgets.length-1; i >= 0; i--) {
      var widget = widgets[i];
      if (!widget.update(g, t))
        widgets.splice(i, 1);
    }
  }

  self.mouseMoveEvent = function(g, x, y) {
      for (var i = widgets.length-1; i >= 0; i--) {
        widgets[i].mouseMoveEvent(g, x, y);
      }
  }


};

/*
var Ninja = Widget.extend({
  init: function(){
    this._super(this);
  },
  dance: function(){
    // Call the inherited version of dance()
    return this._super();
  },
  swingSword: function(){
    return true;
  }
});
*/

var Button = Widget.extend({
	init: function(text, x, y, width, height) {
		this._super(this);
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
		this.clicked = false;
		this.hovered = false;
		this.text = text;
		this.color = "black";
	},
	onDraw: function(g, t) {
		g.ctx.fillStyle = this.color;
		g.ctx.fillRect(this.x, this.y, this.width, this.height);
	},
	onMouseMove: function(g, x, y) {
		if ((x > this.x) && (x<(this.x+this.width)) &&
            (y > this.y) && (y<(this.y+this.height)))
        {
            this.color = "blue";
        }
        else
        {
            this.color = "black";
        }
	}
    
});




