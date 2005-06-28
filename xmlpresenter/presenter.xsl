<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xi="http://www.w3.org/2001/XInclude">
<xsl:output method="xml" indent="yes"/>
<!-- xsl:strip-space elements="*" /-->

<xsl:template match="text()">
	<xsl:value-of select="normalize-space(.)" />
</xsl:template>

<xsl:template match="/slideshow">
	<html>
		<head>
			<xsl:apply-templates select="title" />
			<link rel="stylesheet" type="text/css" href="presenter.css"/>
			<script src="presenter.js" />
		</head>
		<body>
			<!-- Add the controller div -->
			<div class="control-panel">
				<div class="prev">Prev</div>
				<div class="next">Next</div>
			</div>
			<xsl:apply-templates select="slide" />
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
	<xsl:variable name="slideid" select="generate-id(.)" />
	<xsl:variable name="nextslide" select="generate-id(following-sibling::*[1])" />
	<xsl:variable name="visibility">
		<xsl:choose>
			<xsl:when test="count(preceding-sibling::slide) > 0">
				display: none;
			</xsl:when>
			<xsl:otherwise>
				display: block;
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<div id="{$slideid}" class="slide" style="{$visibility}">
		<!-- 
		Somehow apply this slide over whatever theme is selected 
		for this particular slide.
		XXX: Figure out some sort of theme coolness, perhaps
		     xinclude will be useful here?
		-->
		<h1 class="slide-title">
			<xsl:value-of select="title" />
		</h1>
		<xsl:apply-templates select="body"/>
	</div>
</xsl:template>

</xsl:stylesheet>
