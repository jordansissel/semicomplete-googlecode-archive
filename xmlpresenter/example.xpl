<slideshow>
	<title> Example Slideshow/Presentation </title>
	<slide>
		<title> Slide XML </title>
		<body>
			<code>
<![CDATA[
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
		</head>
		<body>
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
	<div id="{$slideid}" class="slide">
		<!-- 
		Somehow apply this slide over whatever theme is selected 
		for this particular slide.
		XXX: Figure out some sort of theme coolness, perhaps
		     xpointer will be useful here?
		-->

		<h1 class="slide-title">
		<xsl:value-of select="title" />
		</h1>
		<!--xsl:copy-of select="body/* | body/text()" /-->
		<xsl:apply-templates />
	</div>
</xsl:template>


</xsl:stylesheet>
]]>
			</code>
		</body>
	</slide>

	<slide>
		<title>Slide 1!</title>
		<body>
			Testing<p/>
			foo
		</body>
	</slide>
</slideshow>
