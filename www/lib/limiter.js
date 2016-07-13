function HashTable(obj)
{
    this.length = 0;
    this.items = {};
    for (var p in obj) {
        if (obj.hasOwnProperty(p)) {
            this.items[p] = obj[p];
            this.length++;
        }
    }

    this.setItem = function(key, value)
    {
        var previous = undefined;
        if (this.hasItem(key)) {
            previous = this.items[key];
        }
        else {
            this.length++;
        }
        this.items[key] = value;
        return previous;
    }

    this.getItem = function(key) {
        return this.hasItem(key) ? this.items[key] : undefined;
    }

    this.hasItem = function(key)
    {
        return this.items.hasOwnProperty(key);
    }
   
    this.removeItem = function(key)
    {
        if (this.hasItem(key)) {
            previous = this.items[key];
            this.length--;
            delete this.items[key];
            return previous;
        }
        else {
            return undefined;
        }
    }

    this.keys = function()
    {
        var keys = [];
        for (var k in this.items) {
            if (this.hasItem(k)) {
                keys.push(k);
            }
        }
        return keys;
    }

    this.values = function()
    {
        var values = [];
        for (var k in this.items) {
            if (this.hasItem(k)) {
                values.push(this.items[k]);
            }
        }
        return values;
    }

    this.each = function(fn) {
        for (var k in this.items) {
            if (this.hasItem(k)) {
                fn(k, this.items[k]);
            }
        }
    }

    this.clear = function()
    {
        this.items = {}
        this.length = 0;
    }
}


var db = new HashTable();

module.exports = function(app) {
    return function(opts) {
        var middleware = function(req, res, next) {
            if (opts.whitelist && opts.whitelist(req)) return next()
            opts.lookup = Array.isArray(opts.lookup) ? opts.lookup : [opts.lookup]
            var lookups = opts.lookup.map(function(item) {
                return item + ':' + item.split('.').reduce(function(prev, cur) {
                    return prev[cur]
                }, req)
            }).join(':')
            var path = opts.path || req.path
            var method = (opts.method || req.method).toLowerCase()
            var key = 'ratelimit:' + path + ':' + method + ':' + lookups

            var now = Date.now();
            var limit = db.getItem(key);

            if (limit == undefined) {
                limit =  {
                    total: opts.total,
                    remaining: opts.total,
                    reset: now + opts.expire
                }
                console.log("Initializing limiter");
            }

            if (now > limit.reset) {
                limit.reset = now + opts.expire;
                limit.remaining = opts.total;
            }
            // do not allow negative remaining
            limit.remaining = Math.max(Number(limit.remaining) - 1, 0);

            db.setItem(key, limit);

            if (!opts.skipHeaders) {
                res.set('X-RateLimit-Limit', limit.total)
                res.set('X-RateLimit-Remaining', limit.remaining)
                res.set('X-RateLimit-Reset', Math.ceil(limit.reset / 1000)) // UTC epoch seconds
            }
            if (limit.remaining) {
                console.log("Limiter granted post request. Remaining: ", limit.remaining);
                return next();
            }
            var after = (limit.reset - Date.now()) / 1000
            if (!opts.skipHeaders) {
                res.set('Retry-After', after)
            }
            res.status(429);
            res.render('429');
        }
        if (opts.method && opts.path) app[opts.method](opts.path, middleware)
        return middleware
    }
}
