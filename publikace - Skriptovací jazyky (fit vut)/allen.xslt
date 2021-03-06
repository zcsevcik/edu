<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:apply-templates select="//p[@class = 'MsoNormal']"/>
  </xsl:template>

  <xsl:template match="span[@style = 'mso-list:Ignore']"/>

  <xsl:template match="a">
    <xsl:value-of select="concat('&quot;', ., '&quot;')"/>
  </xsl:template>

  <xsl:template match="p">
    <xsl:apply-templates select="node()"/>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:template>

  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
