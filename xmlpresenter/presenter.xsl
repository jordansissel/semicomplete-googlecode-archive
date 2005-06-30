<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xi="http://www.w3.org/2001/XInclude">
<xsl:output method="html" indent="yes"/>
<!-- xsl:strip-space elements="*" /-->

<xsl:template match="text()">
	<xsl:value-of select="normalize-space(.)" />
</xsl:template>

<xsl:template match="/slideshow">
	<html>
		<head>
			<xsl:apply-templates select="title" />
			<link rel="stylesheet" type="text/css" href="presenter.css"/>
			<script>
				<xsl:call-template name="generate-slidelist" />
			</script>
			<script src="js/accelimation.js"/>
			<script src="js/dom-drag.js"/>
			<script src="js/presenter.js"/>
		</head>
		<body>

			<!-- Table of Contents pane -->
			<div id="slidelist-panel">
				<xsl:call-template name="generate-slidelist-table" />
			</div>
			<div id="slidelist-expander"></div>

			<div id="slide-container">
			<!-- Add the controller div -->
			<div id="control-panel">
				<!-- left (previus) arrow is disabled by default, becuase we start on the first page -->
				<div id="prev" onclick="button_prev(); return false;">
					<img id="id_button_prev" src="images/leftarrow-disabled.png" alt="Previous Slide" />
				</div>
				<div id="next" onclick="button_next(); return false;">
					<img id="id_button_next" src="images/rightarrow.png" alt="Next Slide" />
				</div>
			</div>

				<xsl:apply-templates select="slide" />
			</div>
		</body>
	</html>
</xsl:template>

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

<!-- Keep text as-is -->
<xsl:template match="/slideshow/slide/body//text()">
	<xsl:value-of select="." />
</xsl:template>

<!-- Special tag 'code' -->
<xsl:template match="/slideshow/slide/body//code">
	<pre class="code">
		<xsl:apply-templates />
	</pre>
</xsl:template>

<xsl:template match="/slideshow/title">
	<title><xsl:value-of select="." /></title>
</xsl:template>

<xsl:template match="slide">
<!--
	<xsl:variable name="slideid" select="generate-id(.)" />
	<xsl:variable name="nextslide" select="generate-id(following-sibling::*[1])" />
-->
	<xsl:variable name="slideid" select="position() - 1" />
	<xsl:variable name="visibility">
		<xsl:choose>
			<xsl:when test="count(preceding-sibling::slide) > 0">display: none;</xsl:when>
			<xsl:otherwise>display: block;</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<div id="slide_{$slideid}" class="slide" style="{$visibility}">
		<!-- 
		Somehow apply this slide over whatever theme is selected 
		for this particular slide.
		XXX: Figure out some sort of theme coolness, perhaps
		     xinclude will be useful here?
		-->
		<div class="slide_content">
			<h1 class="slide-title">
				<a name="slide_{$slideid}" />
				<xsl:value-of select="title" />
			</h1>
			<xsl:apply-templates select="body"/>
		</div>
	</div>
</xsl:template>

<!-- Generate the javascript containing the slides array definition -->
<xsl:template name="generate-slidelist">
	<xsl:text>var slides = new Array();</xsl:text>
	<xsl:for-each select="/slideshow/slide">
		slides.push("slide_<xsl:value-of select="position() - 1"/>");
	</xsl:for-each>
</xsl:template>

<!-- Generate the table of contents -->
<xsl:template name="generate-slidelist-table">
	<ul class="table-of-contents">
	<xsl:for-each select="/slideshow/slide">
		<li id="toc_slide_{position()-1}" onclick="showslide('slide_{position() - 1}'); return false"><xsl:value-of select="title" /></li>
	</xsl:for-each>
	</ul>
</xsl:template>


</xsl:stylesheet>
