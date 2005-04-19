<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:key name="stringy" match="string" use="." />

<xsl:template match="/">
<!--
	<xsl:text>Total tracks: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Tracks']/following-sibling::dict[1]/key)" />
	<xsl:text>&#10;</xsl:text>

	<xsl:text>Total artists: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Artist']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))])" />
	<xsl:text>&#10;</xsl:text>

	<xsl:text>Total albums: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))])" />
	<xsl:text>&#10;</xsl:text>
-->

	<xsl:text>Most used album title: </xsl:text>
	<!--
	<xsl:value-of select="//key[text() = 'Album']/following-sibling::string[1][count(//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))]) > 
	count(//key[text() = 'Album']/following-sibling::string[1][
	]" />
	-->

	<xsl:text>Album: </xsl:text>
	<!-- <xsl:variable name="albumcounts">  -->
		<xsl:for-each select="//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))]">
			<xsl:variable name="albumname" select="text()" />
			<xsl:value-of select="count(//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .)) and text() = $albumname])" />
			<xsl:text>	</xsl:text>
			<xsl:value-of select="$albumname" />
			<xsl:text>&#10;</xsl:text>
		</xsl:for-each>
	<!-- </xsl:variable> -->

	

	<xsl:text>&#10;</xsl:text>
</xsl:template>

</xsl:stylesheet>
