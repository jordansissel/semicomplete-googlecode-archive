var lastslide;
var oldurl;
var oldhash;
var currentslide = 0;

function showslide(id) {
	if (!id) return;
	var x = document.getElementById(id);

	if (!lastslide)
		lastslide = slides[0];

	window.location.hash = id;
	oldurl = location.href;
	//oldhash = window.location.hash;
	document.getElementById(lastslide).style.display = "none";
	x.style.display = "block";
	
	lastslide = id;
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

window.onload = loaded;
