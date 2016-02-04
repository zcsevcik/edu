<?xml version="1.0" encoding="utf-8"?>
<!--
 * news.xslt:      Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
-->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="utf-8" indent="yes"/>

  <xsl:template match="/">
    <xsl:apply-templates select="//item"/>
  </xsl:template>

  <xsl:template match="item">
    <p class="data">
      <xsl:value-of select="@date"/>
    </p>
    <p>
      <xsl:value-of select="text()"/>
    </p>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
