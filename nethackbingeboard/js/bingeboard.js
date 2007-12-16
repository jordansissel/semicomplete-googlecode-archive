(function() {
  if (window.console == undefined) {
    window.console = {
      debug: function(data) { },
      log: function(data) { },
    };
  }

  function random_name() {
    var alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    var length = 10;
    var name = "";
    for (var i = 0; i < length; i++) {
      name += alphabet[Math.floor(Math.random() * alphabet.length)];
    }
    //alert(name)
    return name;
  }

  function BingePlayer(data) {
    for (var key in data) {
      this[key] = data[key];
    }

    this.init();
  }

  BingePlayer.prototype = {
    init: function() {
      var columns = ["name", "race", "class", "gender", "moves", "dlevel", "ulevel", "ac", "wish", "prayer", "deaths"];
      this.cell_elements = {}

      var tr = document.createElement("tr")
      if (this.race == undefined) {
        $(tr).addClass("empty_player");
        this._hidden = true;
      }

      for (var i = 0; i < columns.length; i++) {
        var td = document.createElement("td");
        $(td).addClass(columns[i]);
        $(td).html(this[columns[i]] || "0");
        this.cell_elements[columns[i]] = td;
        $(tr).append(td);
      }

      $("#bingeboard_table").append(tr);
    },

    update: function(key, value) {
      if (this[key] == undefined) {
        //console.log("Unexpected key: " + key + " for player " + this.name);
      }
      this[key] = value;

      if (this.cell_elements[key] == undefined) {
        //console.log("Unknown cell: " + key);
      } else {
        if (this._hidden) {
          this._hidden = False;
          //console.log("removing hidden status")
          $(tr).removeClass("empty_player");
        }
        $(this.cell_elements[key]).html(value);
      }
      // Fire 'updated this key' event?
    },

  }

  function BingeBoard() {
    if (!(this instanceof BingeBoard))
      return new BingeBoard();

    this.players = {};
    this.random_name = random_name()
    while (window[this.random_name])
      this.random_name = random_name();
    window[this.random_name] = this;
    //console.debug(window[this.random_name]);
    this.init();
  }

  BingeBoard.prototype = {
    init: function() {
      var req = new XMLHttpRequest()
      req.multipart = true;
      req.onload = this.xhr_handler;
      callback = this.random_name + ".update_handler";
      //console.debug(this);
      req.open("GET", "/bingerpc?callback=" + callback, true);
      req.send(null);
    },

    xhr_handler: function(e) {
      eval(e.target.responseText);
      //$(".content").html("<pre>" + e.target.responseText + "</pre>");
    },

    update_handler: function(data) {
      // data is an array of dictionaries
      for (var i = 0; i < data.length; i++) {
        info = data[i];
        if (this.players[info.id] == undefined) {
          this.players[info.id] = new BingePlayer(info);
        } else {
          for (var key in info) {
            this.players[info.id].update(key, info[key])
          }
        }
      }
    },
  }

  function handler(xhr) {
    eval(xhr.target.responseText);
  }

  $(document).ready(BingeBoard());

})();
