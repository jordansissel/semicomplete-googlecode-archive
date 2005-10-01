<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xi="http://www.w3.org/2003/XInclude">

<xsl:output method="xml" />

<xsl:param name="filename" select="'UNKNOWN'"/>

<xsl:template match="/">
	<dependencies>
		<!--<xsl:call-template name="processing-instructions" />-->
		<xsl:apply-templates select="//xi:include" />
	</dependencies>
</xsl:template>

<xsl:template match="xi:include">
	<dependency href="{@href}" />
</xsl:template>

</xsl:stylesheet>
