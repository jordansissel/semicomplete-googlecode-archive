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
			<link rel="stylesheet" type="text/css" href="presenter.css" />
		</head>
		<body>
			<xsl:apply-templates select="slide" />
			<xi:include href="test.xml" />
		</body>
	</html>
</xsl:template>

<xsl:template match="/slideshow/title">
	<title><xsl:value-of select="." /></title>
</xsl:template>

<xsl:template match="slide">
	<xsl:variable name="slideid" select="generate-id(.)" />
	<div id="{$slideid}" class="slide">
		<!-- 
		Somehow apply this slide over whatever theme is selected 
		for this particular slide.
		XXX: Figure out some sort of theme coolness, perhaps
		     xpointer will be useful here?
		-->

		<h3>
		<xsl:value-of select="title" />
		</h3>
		<xsl:apply-templates select="body/* | body/text()" />
	</div>
</xsl:template>


</xsl:stylesheet>
