<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:key name="albums" match="text()" use="." />

<xsl:template match="/">
	<xsl:text>Total tracks: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Tracks']/following-sibling::dict[1]/key)" />
	<xsl:text>&#10;</xsl:text>

</xsl:template>

</xsl:stylesheet>
