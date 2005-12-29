<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" />
<xsl:template match="/">
	<div class="project">
		<h4>LINK(article/, <xsl:value-of select="/article/head/title" />)</h4>
		<xsl:value-of select="/article/head/description" />
	</div>
</xsl:template>
</xsl:stylesheet>
