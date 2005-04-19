<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:key name="stringy" match="string" use="." />

<xsl:template match="/">
	<xsl:text>Total tracks: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Tracks']/following-sibling::dict[1]/key)" />
	<xsl:text>&#10;</xsl:text>

	<xsl:text>Total artists: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Artist']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))])" />
	<xsl:text>&#10;</xsl:text>

	<xsl:text>Total albums: </xsl:text>
	<xsl:value-of select="count(//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))])" />
	<xsl:text>&#10;</xsl:text>

	<!-- UNCOMMENT THIS IF YOU WISH TO SEE ALL DISTINCT ALBUM TITLES -->
	<!--
	<xsl:text>Distinct album titles: </xsl:text>
	<xsl:for-each select="//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))]">
		<xsl:value-of select="text()" /> 
		<xsl:text>&#10;</xsl:text>
	</xsl:for-each>
	<xsl:text>&#10;</xsl:text>
	-->

	<xsl:text>Most often used Album: </xsl:text>
	<xsl:text>&#010;</xsl:text>
	<xsl:variable name="albumcounts">
		<xsl:for-each select="//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .))]">
			<xsl:variable name="albumname" select="text()" />
			<xsl:variable name="count" select="count(//key[text() = 'Album']/following-sibling::string[1][generate-id()=generate-id(key('stringy', .)) and text() = $albumname])"/>
			<album name="{text()}" count="{$count}" />
		</xsl:for-each>
	</xsl:variable>

	<xsl:for-each select="$albumcounts/*">
		<xsl:sort select="@count" data-type="number" order="descending" />
		<xsl:if test="position() = 1">
			<xsl:value-of select="@name" />
			<xsl:text>	</xsl:text>
			<xsl:value-of select="@count" />
			<xsl:text>&#10;</xsl:text>
		</xsl:if>
	</xsl:for-each>

	<xsl:text>&#10;</xsl:text>
</xsl:template>

</xsl:stylesheet>
