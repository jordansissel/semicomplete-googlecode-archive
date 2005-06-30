var lastslide;
var oldurl;
var oldhash;
var currentslide = 0;

function showslide(id) {
	if (!id) return;
	var x = document.getElementById(id);
	var toc = document.getElementById("toc_" + id);

	if (!lastslide)
		lastslide = slides[0];

	window.location.hash = id;
	oldurl = location.href;

	if (id != lastslide) {
		document.getElementById(lastslide).style.display = "none";
		document.getElementById("toc_"+lastslide).style.backgroundColor = "";
	}

	x.style.display = "block";
	toc.style.backgroundColor = "#E8FFE8";
	
	lastslide = id;
	currentslide = (id.indexOf("#") == -1 ? "0" : id.substring(id.indexOf("#")+1,id.length));
	fix_buttons();
}

function watchurl() {
	if (oldurl != location.href) {
		//alert("URL CHANGED: \n" + location.href + "\n vs \n" + oldurl);
		oldurl = location.href;

		var i = oldurl.indexOf("#");
		if (i >= 0) {
			var id = oldurl.substring(i + 1, oldurl.length);
			currentslide = id.substring(id.indexOf("_")+1, id.length);
			currentslide = parseInt(currentslide);
			if (isNaN(currentslide)) {
				showslide(slides[0]);
				currentslide = 0;
			} else if (currentslide < 0 || currentslide >= slides.length) {
				showslide(slides[0]);
				currentslide = 0;
			} else {
				showslide(id);
			}
		} else {
			showslide(slides[0]);
			currentslide = 0;
		}
	}
	setTimeout(watchurl, 100);
}

function loaded() {
	watchurl();
	if (!lastslide)
		lastslide = "slide_0";

	var expander = document.getElementById("slidelist-expander");
	Drag.init(expander, null, 0, 130, 0, 0);
	expander.onDrag = function(x, y) {
		setsidelistwidth(x);
	}

	expander.onDragEnd = function(x, y) {
		/* We're done dragging, update the hidden slides with new paddings */
		for (var i = 0; i < slides.length; i++) {
			document.getElementById("slide_"+i).style.paddingLeft = x + 12 + "px";
		}
		setCookie("sidebarwidth", x);
	}
	
	setsidelistwidth(getCookie("sidebarwidth"));
	for (var i = 0; i < slides.length; i++) {
		document.getElementById("slide_"+i).style.paddingLeft = parseInt(getCookie("sidebarwidth")) + 12 + "px";
	}
}

function setsidelistwidth(x) {
	var x = parseInt(x);
	document.getElementById("slidelist-expander").style.left = x + "px";
	document.getElementById("slidelist-panel").style.width = x + "px";
	document.getElementById("control-panel").style.paddingLeft = x + 12 + "px";
	document.getElementById("slide_" + currentslide).style.paddingLeft = x + 12 + "px";
}

function button_next() {
	if (currentslide < slides.length - 1) {
		currentslide++;
		showslide(slides[currentslide]);
		fix_buttons();
	}
}

function button_prev() {
	if (currentslide > 0) {
		currentslide--;
		showslide(slides[currentslide]);
		fix_buttons();
	}
}

function fix_buttons() {
	if (currentslide >= slides.length - 1) {
		document.getElementById("id_button_next").src = "images/rightarrow-disabled.png";
	} else {
		document.getElementById("id_button_next").src = "images/rightarrow.png";
	}

	if (currentslide <= 0) {
		document.getElementById("id_button_prev").src = "images/leftarrow-disabled.png";
	} else {
		document.getElementById("id_button_prev").src = "images/leftarrow.png";
	}
}

var slidelist_state = 0

function toggle_slidelist() {
	var slidelist = document.getElementById("slidelist-panel");
	var control = document.getElementById("control-panel");

	var timelen = 200;
	var dest;
	if (slidelist_state == 0) {
		dest = 150;
		slidelist_state = 1;
	} else {
		dest = 10;
		slidelist_state = 0;
	}

	slidelist.style.width = dest + "px";
	control.style.paddingLeft = (dest) + "px";
	for (var x = 0; x < slides.length; x++) {
		document.getElementById("slide_"+x).style.paddingLeft = (dest + 3) + "px";
	}

	return false;
}

window.onload = loaded;

function setCookie(name, value, expires, path, domain, secure)
{
    document.cookie= name + "=" + escape(value) +
        ((expires) ? "; expires=" + expires.toGMTString() : "") +
        ((path) ? "; path=" + path : "") +
        ((domain) ? "; domain=" + domain : "") +
        ((secure) ? "; secure" : "");
}

function getCookie(name)
{
    var dc = document.cookie;
    var prefix = name + "=";
    var begin = dc.indexOf("; " + prefix);
    if (begin == -1)
    {
        begin = dc.indexOf(prefix);
        if (begin != 0) return null;
    }
    else
    {
        begin += 2;
    }
    var end = document.cookie.indexOf(";", begin);
    if (end == -1)
    {
        end = dc.length;
    }
    return unescape(dc.substring(begin + prefix.length, end));
}
