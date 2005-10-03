<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" />

<xsl:template match="/">
	<xsl:choose>
		<xsl:when test="count(processing-instruction()[name() = 'xml-stylesheet']) > 0">
			<xsl:text>YES</xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text>NO</xsl:text>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>
