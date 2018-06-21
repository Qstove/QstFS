/*
 *
 *  Created on: 02.04.2018
 *      Author: KustovAV
 */

#define FLASH_BASE_ADDR_qstFS	(uint32_t)0xBC200000
#define SEC_SIZE 				(uint32_t)0x40000
#define SECTOR_COUNT 23


/************************
 * Системный 31 сектор. *
 ************************/
#define SEC31_START 			(uint32_t)0xBC7C0000	//служебный
#define SEC32_START 			(uint32_t)0xBC800000	//служебный
#define SYS_SEC_FI_ADDR	 		(uint32_t)0xBC7C0000	//file info
#define SYS_SEC_FI_ADDR_END		(uint32_t)0xBC7C1800

/********************
 * Начала секторов. *
 ********************
#define SEC0_START		(uint32_t)0xBC000000
#define SEC1_START		(uint32_t)0xBC040000
#define SEC2_START		(uint32_t)0xBC080000
#define SEC3_START		(uint32_t)0xBC0C0000		//ЗДЕСЬ ПОКОИТСЯ ОБРАЗ
#define SEC4_START		(uint32_t)0xBC100000
#define SEC5_START		(uint32_t)0xBC140000
#define SEC6_START		(uint32_t)0xBC180000
#define SEC7_START		(uint32_t)0xBC1C0000
*/
#define SEC8_START		(uint32_t)0xBC200000		//отсюда начнется qstFS
#define SEC9_START		(uint32_t)0xBC240000
#define SEC10_START		(uint32_t)0xBC280000
#define SEC11_START		(uint32_t)0xBC2C0000
#define SEC12_START		(uint32_t)0xBC300000
#define SEC13_START		(uint32_t)0xBC340000
#define SEC14_START		(uint32_t)0xBC380000
#define SEC15_START		(uint32_t)0xBC3C0000
#define SEC16_START		(uint32_t)0xBC400000
#define SEC17_START		(uint32_t)0xBC440000
#define SEC18_START		(uint32_t)0xBC480000
#define SEC19_START		(uint32_t)0xBC4C0000
#define SEC20_START		(uint32_t)0xBC500000
#define SEC21_START		(uint32_t)0xBC540000
#define SEC22_START		(uint32_t)0xBC580000
#define SEC23_START		(uint32_t)0xBC5C0000
#define SEC24_START		(uint32_t)0xBC600000
#define SEC25_START		(uint32_t)0xBC640000
#define SEC26_START		(uint32_t)0xBC680000
#define SEC27_START		(uint32_t)0xBC6C0000
#define SEC28_START		(uint32_t)0xBC700000
#define SEC29_START		(uint32_t)0xBC740000
#define SEC30_START		(uint32_t)0xBC780000


typedef struct _filehead {
	uint8_t 	name[8];	//имя
	uint32_t 	CRC;		//CRC данных
	uint32_t 	data_size;	//в байтах
}filehead;


typedef enum {
	OK = 0,					//0 - успех.
	NAME_ALREADY_USED,		//1 - файл с таким именем уже существует.
	INVALID_PARAM,			//2 - ошибочные параметры
	BURNING_ERROR,			//3 - ошибка записи файла.
	NOT_EXISTS,				//4 - файл не найден
	DEFRAG_NOT_NEED,		//5	- дефрагментация не требуется
	MEM_ALLOC_ERR,			//6 - память из пула не выделилась
	ERASE_SEC_ERR,			//7	- проблема с флеш. Память не очистилась.
	SSFI_FULL,				//8	- раздел SSFI переполнен. (файлов больше чем 512)
	CRC_ERR,				//9 - нарушение сохранности файлов, CRC не совпало.
	MEM_FULL				//10- память заполнена
}QST_RES;



typedef struct _system_sector_fileinfo {
	uint8_t 	name[8];
	uint32_t * 	adress;
}sys_sec_fileinfo;		//12 байт (0xC)


typedef struct _read_response {
	uint32_t 	size;
	uint32_t * 	data;
}read_response;


void return_handler(uint32_t result);
uint32_t files_on_flash(uint32_t type);
QST_RES print_SSM(void);
QST_RES init_qstfs(void);
QST_RES create_file(uint8_t * filename, uint32_t size, void * data);
QST_RES delete_file(uint8_t * filename);
QST_RES flash_defrag(void);
QST_RES CRC_check(uint32_t type);
read_response read_file(uint8_t * filename);

