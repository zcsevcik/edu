<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="utf-8"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <!--<xsl:apply-templates select="//div[@class = 'core_text']/node()"/>-->
    <xsl:apply-templates select="//div[@class = 'core_text']//a"/>
  </xsl:template>

  <xsl:template match="a[contains(@href, 'http://scholar.google.com/')]">
    <xsl:text>[scholar]</xsl:text>
    <xsl:value-of select="@href"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template> 

  <xsl:template match="a[.='[bibtext]']">
    <xsl:text>[bib]</xsl:text>
    <xsl:value-of select="@href"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template> 

  <xsl:template match="a"/>
  
  <xsl:template match="h1 | h2"/>
  <xsl:template match="br">
  <xsl:text>&#xa;</xsl:text>
  </xsl:template>
  
  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
