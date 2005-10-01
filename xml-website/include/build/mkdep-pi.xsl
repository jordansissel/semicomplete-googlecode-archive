<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xi="http://www.w3.org/2003/XInclude">

<xsl:output method="text" />

<xsl:template match="/">
<xsl:text>&lt;dependencies&gt;</xsl:text><xsl:call-template name="processing-instructions" /><xsl:text>&lt;/dependencies&gt;</xsl:text>

</xsl:template>

<xsl:template name="processing-instructions">
	<xsl:variable name="deps">
		<xsl:for-each select="processing-instruction()">
			<xsl:if test="name() = 'xml-stylesheet'">
				<xsl:text>&lt;dependency </xsl:text><xsl:value-of select="." /><xsl:text> /&gt;</xsl:text>
			</xsl:if>
		</xsl:for-each>
	</xsl:variable>
	<xsl:value-of select="$deps"/>
</xsl:template>

</xsl:stylesheet>

