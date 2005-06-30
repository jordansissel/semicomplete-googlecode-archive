<?xml version="1.0"?>
<?xml-stylesheet type="text/xml" href="presenter.xsl"?>
<slideshow>
	<title> Example Slideshow/Presentation </title>
	<slide>
		<title> XPL Slideshow/Presentation Example </title>
		<body>
			This is an xml-based website slideshow tool.<p/>

			Features:
			<ul>
			<li> Slide content <i>is</i> HTML!</li>
			<li> The XML format for the slideshow is easy to use</li>
			<li> Slides can be longer than the viewport of the browser</li>
			<li> Slides can be instantly referenced using anchors, such as #slide_3
				<ul> <li>
					For example, you can visit slide #2 by
					<a href="#slide_2">clicking here</a>
				</li> </ul>
			</li>
			<li> The slides pane on the left remembers where you put it across page loads (cookies!) </li>

			<li> Mozilla browsers have history support. The "back" button should work!</li>
			<li> Navigation buttons at the top-left of the page let you control the direction of your slideshow.</li>
			</ul>

			<p/>

			If your browser doesn't suck, you should see a thin bar at the left of the window saying 'SLIDES'. You can drag this outward to reveal a list of all the slides in your presentation. The current one is highlighted. You can click on others in this list and they will be shown.

			<p/>
			Browser Support List:
			<ul>
			<li> Mozilla Family: 100% works </li>
			<li> Konqueror 3.4.0: 100% works </li>
			<li> Opera 8: Everything works except browser back/forward (history) </li>
			<li> Internet Explorer 5: Navigation works kind of works, something's busted. </li>
			<li> Safari: Doesn't work at all. Safari is crap anyway, don't use it. </li>
			</ul>
		</body>
	</slide>

	<slide>
		<title>Example Slide That Is Too Large </title>
		<body>
		
			<h3> This slide is probably too large to fit in the view of your browser window, so you can scroll down. </h3>
			
			<ul>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> TESTING </li>
				<li> FOO BAR BAZ </li>
				<li> FOO BAR BAZ </li>
				<li> TESTING </li>
				<li> TESTING </li>
			</ul>
		</body>
	</slide>

	<slide>
		<title>Code snippet example</title>
		<body>
			<h3> Here you can see it's easy to insert code snippets.</h3>
			<code><![CDATA[
<!-- Copy the tree downward, we'll apply specific templates as necessary -->
<xsl:template match="/slideshow/slide/body//*[name() != 'code']">
   <xsl:copy>
      <xsl:for-each select="@*">
         <xsl:variable name="key" select="name()" />
         <xsl:attribute name="{$key}">
            <xsl:value-of select="normalize-space(.)" />
         </xsl:attribute>
      </xsl:for-each>
      <xsl:apply-templates />
   </xsl:copy>
</xsl:template>
			]]></code>

			Here's another:
			<code><![CDATA[
/*
 * joystick daemon
 *
 * Whispers sweet nothings to uhid devices, perhaps they'll be
 * usb gamepad/joysticks. We'll never know, will we?
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <err.h>
#include <ctype.h>
#include <errno.h>
#include <usbhid.h>
#include <dev/usb/usb.h>
#include <dev/usb/usbhid.h>

static char *prog = NULL;
static int reportid;

void dataloop(int f, report_desc_t rd);
void rev(struct hid_item **p);
void prdata(u_char *buf, struct hid_item *h);
int gotname(char *n);

int
main(int argc, char **argv)
{
   int ch;
   int f;
   report_desc_t r;
   char *dev = "/dev/uhid0";
   char *table = NULL;

   while ((ch = getopt(argc, argv, "d:")) != -1) {
      switch (ch) {
         case 'd':
            dev = optarg;
            break;
         default:
            printf("Unknown option '%c'\n", ch);
      }
   }

   argc -= optind;
   argv += optind;

   /* XXX: Implement usage() */
   //if (dev == NULL) usage();

   hid_init(table);

   f = open(dev, O_RDWR);
   if (f < 0)
      err(1, "%s", dev);

   r = hid_get_report_desc(f);
   if (r == 0)
      err(1, "USB_GET_REPORT_DESC failed.");

   /* Main loop */
   dataloop(f, r);

   /* We shouldn't really get here... */

   hid_dispose_report_desc(r);
   return 0;
}
			]]></code>
		</body>
	</slide>

	<slide>
		<title> The end :(</title>
		<body>
			This is just a very simple example of what you can do with this tool. 
			<p/>

			The source to this presentation can be downloaded here:<br/>
			<i>NOTE: If your browser renders example.xpl as a presentation, that's because your browser supports local xslt translations (which is a very nice feature!). Simply download it manually or use 'view source' from your browser to see the underlying XML.</i>
			<ul>
				<li><a href="/~psionic/projects/xmlpresenter/example.xpl">example.xpl</a></li>
				<li><a href="/~psionic/projects/xmlpresenter/presenter.xsl">presenter.xsl</a></li>
				<li><a href="/~psionic/projects/xmlpresenter/presenter.css">presenter.css</a></li>
				<li><a href="/~psionic/projects/xmlpresenter/presenter.js">presenter.js</a></li>
			</ul>
		</body>
	</slide>

</slideshow>
