<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xi="http://www.w3.org/2003/XInclude" version="1.0">
<xsl:output method="xml" indent="yes"/>

<xsl:template match="/">
	<html>
		<!-- Header: Logo, etc --><head>
	<title> Testing </title>
</head>
		<body>
			<test>
asdf
</test>
			<xsl:value-of select="/a"/>
		</body>
	</html>
</xsl:template>

</xsl:stylesheet>
