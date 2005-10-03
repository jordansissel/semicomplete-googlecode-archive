<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		  xmlns:xi="http://www.w3.org/2003/XInclude">

<xsl:output method="xml" indent="yes" />

<xsl:template match="/">
	<html>
		<!-- HEAD -->
		<xi:include href="include/header.xml" />
		<!-- HEAD -->
		<body>
			<!-- Logo --> 
			<xi:include href="include/logo.xml" />
			<!-- End Logo --> 

			<!-- Menu -->
			<xi:include href="include/menu.xml" />
			<!-- End Menu -->

			<!-- Body -->
			<div id="content">
				<xsl:apply-templates select="/page/body" />
			</div>
			<!-- End Body -->
		</body>
	</html>
</xsl:template>

<xsl:template match="/page/body//*">
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

</xsl:stylesheet>

