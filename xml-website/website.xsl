<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		  xmlns:xi="http://www.w3.org/2003/XInclude">
<xsl:output method="xml" indent="yes" />

<xsl:template match="/">
	<html>
		<xi:include href="header.xml" />
		<body>
			<xi:include href="menu.xml" />
			<xsl:value-of select="/a" />
		</body>
	</html>
</xsl:template>

</xsl:stylesheet>

