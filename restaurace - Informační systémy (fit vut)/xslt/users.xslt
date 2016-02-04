<?xml version="1.0" encoding="utf-8"?>
<!--
 * users.xslt:     Restaurace (IIS 2012)
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

  <xsl:template match="user">
    <tr>
      <td nowrap="nowrap">
        <xsl:element name="input">
          <xsl:attribute name="type">image</xsl:attribute>
          <xsl:attribute name="src">images/cross.png</xsl:attribute>
          <xsl:attribute name="title">Smazat</xsl:attribute>
          <xsl:attribute name="alt">Smazat</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="onclick">
            delete_user(this);
          </xsl:attribute>
        </xsl:element>
        <b><xsl:value-of select="@login"/></b>
      </td>
      <td>
        <xsl:element name="select">
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="title">Role</xsl:attribute>
          <xsl:attribute name="onchange">
            change_role(this);
          </xsl:attribute>
          <xsl:element name="option">
            <xsl:attribute name="value">cisnik</xsl:attribute>
            <xsl:if test="@role = 'cisnik'">
              <xsl:attribute name="selected">selected</xsl:attribute>
            </xsl:if>
            <xsl:text>Číšník</xsl:text>
          </xsl:element>
          <xsl:element name="option">
            <xsl:attribute name="value">admin</xsl:attribute>
            <xsl:if test="@role = 'admin'">
              <xsl:attribute name="selected">selected</xsl:attribute>
            </xsl:if>
            <xsl:text>Správce</xsl:text>
          </xsl:element>
          <xsl:element name="option">
            <xsl:attribute name="value">sefkuchar</xsl:attribute>
            <xsl:if test="@role = 'sefkuchar'">
              <xsl:attribute name="selected">selected</xsl:attribute>
            </xsl:if>
            <xsl:text>Šéfkuchař</xsl:text>
          </xsl:element>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="title">Jmeno</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="value"><xsl:value-of select="@jmeno"/></xsl:attribute>
          <xsl:attribute name="onchange">
            change_name(this);
          </xsl:attribute>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="title">Telefon</xsl:attribute>
          <xsl:attribute name="style">width: 80px;</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="value"><xsl:value-of select="@telefon"/></xsl:attribute>
          <xsl:attribute name="onchange">
            change_phone(this);
          </xsl:attribute>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="title">Mail</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="value"><xsl:value-of select="@mail"/></xsl:attribute>
          <xsl:attribute name="onchange">
            change_mail(this);
          </xsl:attribute>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
          <xsl:attribute name="type">button</xsl:attribute>
          <xsl:attribute name="title">Vygenerovat nové heslo</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@login"/></xsl:attribute>
          <xsl:attribute name="onclick">
            new_pwd(this);
          </xsl:attribute>
        </xsl:element>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="/">
      <table>
        <tr>
          <th>Login</th>
          <th>Role</th>
          <th>Jméno</th>
          <th>Telefon</th>
          <th>Mail</th>
          <th>Heslo</th>
        </tr>
        <xsl:apply-templates select="//user"/>
        <tr>
          <td colspan="6" class="spacer">
          </td>
        </tr>
        <tr>
          <td colspan="6">
            <xsl:element name="input">
              <xsl:attribute name="type">text</xsl:attribute>
              <xsl:attribute name="title">Login</xsl:attribute>
              <xsl:attribute name="value">Nový účet</xsl:attribute>
              <xsl:attribute name="style">width: 150px; font-style: italic; color: gray;</xsl:attribute>
              <xsl:attribute name="onfocus">
                this.style.color = '';
                this.style.fontStyle = '';
                this.value = '';
              </xsl:attribute>
              <xsl:attribute name="onchange">
                new_user(this);
              </xsl:attribute>
            </xsl:element>
          </td>
        </tr>
      </table>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
