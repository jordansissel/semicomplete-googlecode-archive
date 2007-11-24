
function foo(e) {
  alert(e);
}

var jquerycmd;

jquerycmd = {
  onload: function(e) {
    this.initialized = true;
    this.browser_loaded = false;
    //dump("onload\n");

    var nsCommandLine = window.arguments[0];
    nsCommandLine = nsCommandLine.QueryInterface(Components.interfaces.nsICommandLine);
    this.url = nsCommandLine.handleFlagWithParam("url", false);
    this.query = nsCommandLine.handleFlagWithParam("query", false);

    this.browser = document.getElementById("main-browser");
    this.browser.addEventListener("pageshow", this.onpageshow, false);
    this.browser.loadURI(this.url, null, null);
  },

  onpageshow: function(e) {
    //dump("Checking page\n");

    // Sometimes the pageshow event is triggered twice?
    if (jquerycmd.browser_loaded == false) {
      jquerycmd.browser_loaded = true;
      return;
    }


    var result = $(jquerycmd.query, this.contentDocument);
    result.each(function(el) {
      var output = "<" + this.nodeName;
      for (var i = 0; i < this.attributes.length; i++) {
        var attr = this.attributes[i];
        output += " " + attr.name + "='" + attr.value + "'";
      }
      if (this.childNodes.length > 0) {
        output += ">";
        output += this.innerHTML;
        output += "</" + this.nodeName + ">";
      } else {
        output += " />";
      }
      dump(output + "\n");
    });

    window.close();
  },
};

window.addEventListener("load", function(e) { jquerycmd.onload(e); }, false);
