/**
  *****************************************************************************
  * @lib:			character_encoding
  * @author			Lilu
  * @version		V1.0.0
  * @data			2015-8-14
  * @Brief			character_encoding��֧�ֶ�����Ե�ANSI������unicode������໥ת��
  *					���롢������ݵĴ�С��ģʽ������
  *					unicode�����ʽ֧�֣�UTF-8��UTF-16������unicodeĬ��ָ��UTF-16��
  *					UNICODEָUCS-2��������֧��BMP֮��Ĳ������ַ�
  *					(ע:��������UNICODE,UTF-16,UCS-2���ߺ����ͬ)
  ******************************************************************************
 */
#ifndef __CHARACTER_ENCODING_H__
#define __CHARACTER_ENCODING_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus	
	
#include "type.h"

#define _CODE_PAGE	936
/* This option specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   1    - ASCII (No extended character. Non-LFN cfg. only)
/   437  - U.S.
/   720  - Arabic
/   737  - Greek
/   775  - Baltic
/   850  - Multilingual Latin 1
/   852  - Latin 2
/   855  - Cyrillic
/   857  - Turkish
/   858  - Multilingual Latin 1 + Euro
/   862  - Hebrew
/   866  - Russian
/   874  - Thai
/   932  - Japanese Shift_JIS (DBCS)
/   936  - Simplified Chinese GBK (DBCS)
/   949  - Korean (DBCS)
/   950  - Traditional Chinese Big5 (DBCS)
*/

#define	ANSI_TO_UNICODE		0x01//�ַ�����ת������
#define	UNICODE_TO_ANSI		0x00

#define	UTF8				0x02//UNICODE����õı�����ʽ��Ĭ��ΪUTF16��UCS-2
#define	UTF16				0x00

#define	BIG_ENDIAN_IN		0x04//�����ַ�����С�ˣ�Ĭ��ΪС��
#define	LITTLE_ENDIAN_IN	0x00

#define	BIG_ENDIAN_OUT		0x08//����ַ�����С�ˣ�Ĭ��ΪС��
#define	LITTLE_ENDIAN_OUT	0x00

/**
 * @brief
 *   �ַ������ַ���ת����UNICODE��ANSI��ת
 * @param	dst			[input]Ŀ���ַ���ָ��
 * @param	dst_len		[input]Ŀ���ַ������ȣ�byte��λ
 * @param	src			[input]Դ�ַ���ָ��
 * @param	dst_len		[input]Դ�ַ������ȣ�byte��λ
 * @param	type		[input]ת�����ͣ��μ�h�ļ��к궨��
 *
 * @return
 *   Ŀ���ַ�����ʵ�ʳ��ȣ�0��ʾת��ʧ��
 * @note	ת������UNICODE_TO_ANSI��ANSI_TO_UNICODE����ת������unicode���뷽ʽ����С��Ϊ��ѡ����
 *			��:	UNICODE_TO_ANSI | UTF8 | BIG_ENDIAN_OUTΪutf8������ANSIת�������롢�����Ϊ���ģʽ
 *				UNICODE_TO_ANSI Ĭ��ΪUTF16��ANSIת�������������ΪС��ģʽ
 *				UTF8���� û�д�С�˵�����
 */
unsigned int character_encoding_string_convert(unsigned char* dst, unsigned int dst_len,
							unsigned char* src, unsigned int src_len, unsigned int type);

/**
 * @brief
 *	�ַ�����ת����UNICODE��ANSI��ת
 * @param	chr		Դ�ַ�
 * @param	dir		ת������UNICODE_TO_ANSI��ANSI_TO_UNICODE
 *
 * @return
 *	Ŀ���ַ���0��ʾת��ʧ��
 */
unsigned short character_encoding_convert (unsigned short chr, unsigned int	dir);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__CHARACTER_ENCODING_H__*/


