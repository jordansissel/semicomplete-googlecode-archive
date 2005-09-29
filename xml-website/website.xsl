<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		  xmlns:xi="http://www.w3.org/2003/XInclude">

<xsl:output method="xml" indent="yes" />

<xsl:template match="/">
	<html>
		<xi:include href="include/header.xmli" />
		<body>
			<!-- Menu -->
			<xi:include href="include/menu.xmli" />
			<!-- End Menu -->

			<!-- Body -->
			<xsl:apply-templates select="/body" />
			<!-- End Body -->
		</body>
	</html>
</xsl:template>

<xsl:template match="/body//*">
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

