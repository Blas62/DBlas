//***************************************************************************************
#pragma once
//---------------------------------------------------------------------------------------
#include <cstdint>
#include <filesystem>
#include "DError.h"
#include "DMemory.h"
//---------------------------------------------------------------------------------------
namespace blas
{
	namespace utility
	{
		class DFormatConverter : public blas::utility::DError
		{
		public:
			/**
			 * @brief Costruttore predefinito della classe DFormatConverter.
			 *
			 * @details Inizializza un oggetto DFormatConverter impostando la tabella di conversione
			 *          per la codifica Base64. La tabella viene allocata dinamicamente e popolata
			 *          con i 64 caratteri standard della codifica Base64 secondo RFC 4648.
			 *          La memoria è gestita internamente tramite allocate_memory<char>() e deve essere
			 *          rilasciata dal distruttore per evitare perdite di memoria.
			 *
			 * @note La tabella di codifica Base64 creata da questo costruttore segue l'alfabeto standard:
			 *       "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".
			 *       Eventuali modifiche a questa tabella devono essere effettuate con cautela,
			 *       poiché possono compromettere la compatibilità con altre implementazioni Base64.
			 *
			 * @warning L'inizializzazione non è thread-safe. Se l'oggetto viene usato in contesti concorrenti,
			 *          occorre implementare opportune misure di sincronizzazione.
			 *
			 * @see https://datatracker.ietf.org/doc/html/rfc4648
			 * @since 1.0
			 */
			DFormatConverter();
			/**
			 * @brief Distruttore della classe DFormatConverter.
			 *
			 * @details Rilascia le risorse allocate dinamicamente durante il ciclo di vita
			 *          dell'oggetto, in particolare la memoria riservata per la tabella
			 *          di codifica Base64 inizializzata nel costruttore.
			 *          Dopo la deallocazione, il puntatore viene impostato a nullptr per
			 *          prevenire riferimenti pendenti (dangling pointers).
			 *
			 * @note La funzione verifica che il puntatore alla tabella non sia nullptr
			 *       prima di tentare la deallocazione, evitando comportamenti indefiniti.
			 *
			 * @warning Se la tabella Base64 è stata modificata esternamente o riallocata
			 *          senza aggiornare il puntatore interno, la deallocazione potrebbe
			 *          causare accessi a memoria non valida.
			 *
			 * @see DFormatConverter::DFormatConverter()
			 * @since 1.0
			 */	
			~DFormatConverter();
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::string Base64);
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::filesystem::path InName,
				std::filesystem::path OutName);
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::string Base64, bool URL, bool Padded);
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::filesystem::path InName,
				std::filesystem::path OutName, bool URL, bool Padded);
			/**
			 * @brief Decodifica un buffer Base64 in binario usando impostazioni predefinite.
			 *
			 * @details Overload semplificato di @c base64_to_binary() che utilizza:
			 *          - @p URL = false (alfabeto Base64 standard, con '+' e '/')
			 *          - @p Padded = true (input con padding '=' e lunghezza multipla di 4)
			 *
			 *          È equivalente a chiamare:
			 *          @code
			 *          base64_to_binary(Base64, false, true);
			 *          @endcode
			 *
			 * @param[in] Base64 Coppia <tt>{char* buffer, uint32_t length}</tt> contenente
			 *                   la stringa Base64 da decodificare e la sua lunghezza.
			 *
			 * @return Coppia <tt>{uint8_t* data, uint32_t size}</tt> con un puntatore al buffer
			 *         binario allocato dinamicamente e la sua dimensione in byte.
			 *         In caso di errore restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @pre  @p Base64.first non deve essere @c nullptr.
			 * @post Restituisce un buffer valido da liberare con @c deallocate_memory().
			 *
			 * @note Utile per la decodifica rapida quando non sono richieste opzioni speciali
			 *       (URL-safe o senza padding).
			 *
			 * @see base64_to_binary(std::pair<char*, uint32_t>, bool, bool)
			 * @since 1.0
			 */
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::pair<char*, uint32_t>Base64);
			/**
			 * @brief Converte dati codificati in Base64 nel corrispondente buffer binario.
			 *
			 * @details Decodifica un buffer di testo Base64 (alfabeto standard o URL-safe)
			 *          producendo il contenuto binario originale. Se @p URL è true viene
			 *          usato l'alfabeto URL-safe ( '-' e '_' ), altrimenti quello standard
			 *          ( '+' e '/' ). Se @p Padded è true l'input deve avere lunghezza
			 *          multipla di 4 e può contenere caratteri '=' di padding; in caso
			 *          contrario l'operazione fallisce. La variante senza padding
			 *          (@p Padded == false) è marcata come TODO e non è attualmente
			 *          supportata: la funzione potrebbe non gestire correttamente questo
			 *          caso.
			 *
			 *          L'implementazione:
			 *          - Imposta i caratteri 62 e 63 di @c Base64Table in base al flag
			 *            @p URL (side-effect sull'oggetto).
			 *          - Verifica i vincoli di lunghezza quando @p Padded è true.
			 *          - Calcola la dimensione massima del risultato come
			 *            <tt>(Base64.second / 4) * 3</tt>.
			 *          - Decodifica blocchi da 4 caratteri producendo 3 byte alla volta
			 *            tramite @c block_to_binary().
			 *
			 *          In caso di errore viene catturata l'eccezione, impostato lo stato
			 *          d'errore tramite @c set_error(), e la funzione restituisce
			 *          <tt>{ nullptr, 0 }</tt>.
			 *
			 * @param[in] Base64 Coppia <tt>{char* buffer, uint32_t length}</tt> con il
			 *                   testo Base64 in input. Il buffer non è di proprietà della
			 *                   funzione e non viene modificato.
			 * @param[in] URL    Se true usa l'alfabeto Base64 URL-safe ('-' e '_');
			 *                   se false usa l'alfabeto standard ('+' e '/').
			 * @param[in] Padded Se true l'input deve essere correttamente padding-ato ('=')
			 *                   e avere lunghezza multipla di 4. La modalità senza padding
			 *                   non è al momento completamente implementata.
			 *
			 * @return Coppia <tt>{uint8_t* data, uint32_t size}</tt> con un puntatore a un
			 *         buffer allocato dinamicamente contenente i dati decodificati e il
			 *         numero di byte effettivamente scritti. In caso di errore restituisce
			 *         <tt>{ nullptr, 0 }</tt>.
			 *
			 * @pre  @c Base64Table deve essere stato inizializzato dal costruttore;
			 *       @p Base64.first non deve essere @c nullptr; se @p Padded è true,
			 *       @p Base64.second % 4 == 0.
			 * @post Se la decodifica ha successo, viene restituito un buffer valido da
			 *       deallocare con @c deallocate_memory(). I caratteri 62 e 63 di
			 *       @c Base64Table riflettono lo stato del flag @p URL.
			 *
			 * @note La memoria del risultato è di proprietà del chiamante e deve essere
			 *       liberata con @c deallocate_memory(). La funzione modifica
			 *       @c Base64Table (side-effect) per adattare l'alfabeto.
			 *
			 * @warning Non thread-safe se più thread invocano la funzione con valori di
			 *          @p URL differenti, poiché @c Base64Table viene modificata in place.
			 *          La modalità @p Padded == false è da considerarsi non supportata.
			 *
			 * @complexity O(n) sul numero di caratteri Base64 in input.
			 *
			 * @see block_to_binary, deallocate_memory, DFormatConverter::DFormatConverter()
			 * @since 1.0
			 */
			std::pair<uint8_t*, uint32_t>base64_to_binary(std::pair<char*, uint32_t>Base64, bool URL, bool Padded);
			/**
			 * @brief Codifica un buffer binario in Base64 usando impostazioni predefinite.
			 *
			 * @details Overload semplificato di @c binary_to_base64() che utilizza:
			 *          - @p URL = false (alfabeto Base64 standard, con '+' e '/')
			 *          - @p Padded = true (output con padding '=' e lunghezza multipla di 4)
			 *
			 *          È equivalente a chiamare:
			 *          @code
			 *          binary_to_base64(Binary, false, true);
			 *          @endcode
			 *
			 * @param[in] Binary Coppia <tt>{uint8_t* buffer, uint32_t length}</tt> contenente
			 *                   i dati binari da codificare e la loro lunghezza in byte.
			 *
			 * @return Coppia <tt>{char* data, uint32_t size}</tt> contenente un puntatore a un
			 *         buffer allocato dinamicamente con la stringa Base64 e la sua lunghezza effettiva.
			 *         In caso di errore restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @pre  @p Binary.first non deve essere @c nullptr.
			 * @post Restituisce un buffer valido da liberare con @c deallocate_memory().
			 *
			 * @note Utile per la codifica rapida quando non sono richieste opzioni speciali
			 *       (URL-safe o senza padding).
			 *
			 * @see binary_to_base64(std::pair<uint8_t*, uint32_t>, bool, bool)
			 * @since 1.0
			 */
			std::pair<char*, uint32_t>binary_to_base64(std::pair<uint8_t*, uint32_t>Binary);
			std::pair<char*, uint32_t>binary_to_base64(std::string Binary); 
			std::pair<char*, uint32_t>binary_to_base64(std::string Binary, bool URL, bool Padded);
			/**
			 * @brief Converte un buffer binario nel corrispondente testo codificato Base64.
			 *
			 * @details Codifica un buffer di dati binari in una stringa Base64 utilizzando
			 *          l'alfabeto standard o quello URL-safe. Se @p URL è true, i caratteri
			 *          '+' e '/' vengono sostituiti rispettivamente con '-' e '_', come
			 *          previsto dallo schema URL-safe definito in RFC 4648.
			 *          La lunghezza del testo codificato viene calcolata come
			 *          <tt>(Binary.second / 3 + 1) * 4</tt>, allocando lo spazio necessario
			 *          tramite @c allocate_memory<char>().
			 *
			 *          L'algoritmo procede in blocchi da 3 byte binari, convertendoli in
			 *          4 caratteri Base64 tramite la funzione @c block_to_base64().
			 *          Se @p Padded è false, eventuali caratteri '=' di padding vengono
			 *          rimossi in coda e la lunghezza complessiva ridotta di conseguenza.
			 *
			 *          In caso di eccezione, viene impostato lo stato di errore tramite
			 *          @c set_error() e restituito <tt>{nullptr, 0}</tt>.
			 *
			 * @param[in] Binary Coppia <tt>{uint8_t* buffer, uint32_t length}</tt> con i dati
			 *                   binari da codificare. Il buffer non è di proprietà della
			 *                   funzione e non viene modificato.
			 * @param[in] URL    Se true, usa l'alfabeto URL-safe ('-' e '_');
			 *                   se false, usa l'alfabeto standard ('+' e '/').
			 * @param[in] Padded Se true, il testo Base64 generato includerà padding '='
			 *                   fino a lunghezza multipla di 4; se false, il padding
			 *                   verrà rimosso.
			 *
			 * @return Coppia <tt>{char* data, uint32_t size}</tt> contenente un puntatore a
			 *         un buffer allocato dinamicamente con la stringa Base64 e la sua
			 *         lunghezza effettiva (senza terminatore di stringa '\0').
			 *         In caso di errore, restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @pre  @c Base64Table deve essere stato inizializzato dal costruttore.
			 *       @p Binary.first non deve essere @c nullptr.
			 * @post Restituisce un buffer valido da deallocare con @c deallocate_memory().
			 *       I caratteri 62 e 63 di @c Base64Table riflettono lo stato del flag @p URL.
			 *
			 * @note La memoria del risultato è di proprietà del chiamante e deve essere
			 *       liberata con @c deallocate_memory().
			 *       La funzione modifica @c Base64Table (side-effect) per adattare l'alfabeto.
			 *
			 * @warning Non thread-safe se più thread invocano la funzione con valori di
			 *          @p URL differenti, poiché @c Base64Table viene modificata in place.
			 *
			 * @complexity O(n) sul numero di byte binari in input.
			 *
			 * @see block_to_base64, deallocate_memory, DFormatConverter::base64_to_binary()
			 * @since 1.0
			 */
			std::pair<char*, uint32_t>binary_to_base64(std::pair<uint8_t*, uint32_t>Binary, bool URL, bool Padded);
			/**
			 * @brief Converte un buffer binario nella sua rappresentazione testuale esadecimale.
			 *
			 * @details Trasforma ogni byte di un buffer binario in due caratteri ASCII
			 *          corrispondenti alla sua rappresentazione in notazione esadecimale
			 *          (alfabeto 0-9, A-F).
			 *          L'output non include prefissi come "0x" e non contiene spazi o separatori.
			 *
			 *          La lunghezza del testo generato è pari a <tt>Binary.second * 2</tt>,
			 *          poiché ogni byte viene rappresentato da due caratteri.
			 *          L'allocazione della memoria per il risultato avviene tramite
			 *          @c allocate_memory<char>() e deve essere gestita dal chiamante.
			 *
			 *          L'algoritmo:
			 *          - Cicla su ogni byte di input.
			 *          - Estrae il nibble alto e il nibble basso usando operazioni bitwise.
			 *          - Converte ciascun nibble in un carattere ASCII tramite
			 *            @c nibble_to_char().
			 *          - Popola il buffer di output con la sequenza risultante.
			 *
			 *          In caso di eccezione, imposta lo stato di errore con @c set_error()
			 *          e restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @param[in] Binary Coppia <tt>{uint8_t* buffer, uint32_t length}</tt> con il
			 *                   buffer binario in input e la sua lunghezza in byte.
			 *
			 * @return Coppia <tt>{char* data, uint32_t size}</tt> contenente un puntatore
			 *         a un buffer allocato dinamicamente con la stringa esadecimale e la
			 *         sua lunghezza effettiva (senza terminatore di stringa '\0').
			 *         In caso di errore restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @pre  @p Binary.first non deve essere @c nullptr.
			 * @post Restituisce un buffer valido di lunghezza esatta pari a
			 *       <tt>Binary.second * 2</tt>, da liberare con @c deallocate_memory().
			 *
			 * @note La memoria del risultato è di proprietà del chiamante e deve essere
			 *       liberata con @c deallocate_memory().
			 *
			 * @warning Non thread-safe se @c nibble_to_char() o @c allocate_memory()
			 *          non sono thread-safe.
			 *
			 * @complexity O(n) sul numero di byte in input.
			 *
			 * @see nibble_to_char, deallocate_memory, text_to_binary
			 * @since 1.0
			 */
			std::pair<char*,uint32_t>binary_to_text(std::pair<uint8_t*, uint32_t>Binary);
			/**
			 * @brief Converte una stringa esadecimale nel corrispondente buffer binario.
			 *
			 * @details Decodifica una sequenza di caratteri ASCII che rappresentano byte
			 *          in formato esadecimale (alfabeto 0-9, A-F, case-insensitive)
			 *          e restituisce il buffer binario corrispondente.
			 *          Ogni coppia di caratteri dell'input viene interpretata come un
			 *          singolo byte tramite la funzione @c two_char_to_binary().
			 *
			 *          La funzione verifica preliminarmente che la lunghezza dell'input
			 *          sia pari (multipla di 2), altrimenti solleva un'eccezione
			 *          @c std::length_error.
			 *          Lo spazio di memoria per il risultato viene allocato dinamicamente
			 *          tramite @c allocate_memory<uint8_t>() e deve essere gestito dal
			 *          chiamante.
			 *
			 *          In caso di eccezione, lo stato d'errore viene impostato tramite
			 *          @c set_error() e la funzione restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @param[in] Text Coppia <tt>{char* buffer, uint32_t length}</tt> con la stringa
			 *                 esadecimale in input e la sua lunghezza in caratteri.
			 *                 Il buffer non viene modificato e non è di proprietà della funzione.
			 *
			 * @return Coppia <tt>{uint8_t* data, uint32_t size}</tt> contenente un puntatore
			 *         a un buffer allocato dinamicamente con i dati binari decodificati e
			 *         la loro lunghezza in byte.
			 *         In caso di errore, restituisce <tt>{nullptr, 0}</tt>.
			 *
			 * @pre  @p Text.first non deve essere @c nullptr.
			 *       @p Text.second deve essere pari (multiplo di 2).
			 * @post Restituisce un buffer binario valido, da liberare con @c deallocate_memory().
			 *
			 * @note La funzione non valida il contenuto dei caratteri esadecimali
			 *       oltre alla verifica della lunghezza. Eventuali caratteri non validi
			 *       possono causare comportamenti indefiniti in @c two_char_to_binary().
			 *
			 * @warning Non thread-safe se @c allocate_memory() o @c two_char_to_binary()
			 *          non sono thread-safe.
			 *
			 * @complexity O(n/2) rispetto al numero di caratteri in input.
			 *
			 * @see binary_to_text, two_char_to_binary, deallocate_memory
			 * @since 1.0
			 * 
			 */
			std::pair<uint8_t*, uint32_t>text_to_binary(std::pair<char*, uint32_t>Text);
			const bool is_binary(const uint8_t C);
		protected:
			// Base64
			/**
			 * @brief Converte un blocco di 4 caratteri Base64 in un massimo di 3 byte binari.
			 *
			 * @details Decodifica un singolo blocco di input codificato in Base64 (standard o URL-safe)
			 *          e scrive i byte binari risultanti in @p OutBlock.
			 *          Il blocco di input deve avere una lunghezza di 4 caratteri (eventualmente
			 *          con caratteri di padding '=' in coda). La funzione gestisce i casi di
			 *          padding parziale restituendo il numero corretto di byte effettivamente decodificati.
			 *
			 *          L'algoritmo esegue:
			 *          - Estrazione e decodifica di ciascun carattere Base64 tramite @c decode_base64().
			 *          - Operazioni bitwise di shift e OR per ricostruire i byte originali.
			 *          - Controllo del carattere '=' per determinare se fermarsi prima dei 3 byte completi.
			 *
			 * @param[in]  InBlock   Puntatore a un array di 4 caratteri Base64 da decodificare.
			 *                       Può contenere '=' come padding nelle ultime posizioni.
			 * @param[out] OutBlock  Puntatore a un buffer di almeno 3 byte dove verranno scritti
			 *                       i dati binari decodificati.
			 *
			 * @return Numero di byte effettivamente decodificati:
			 *         - 3 se il blocco non contiene padding
			 *         - 2 se è presente un solo carattere '=' di padding
			 *         - 1 se sono presenti due caratteri '=' di padding
			 *
			 * @pre  @p InBlock deve puntare a un buffer di almeno 4 caratteri validi Base64 o '='.
			 *       @p OutBlock deve puntare a un buffer con almeno 3 byte disponibili.
			 * @post I primi N byte di @p OutBlock con N pari al valore di ritorno contengono
			 *       la sequenza binaria decodificata.
			 *
			 * @note La funzione non alloca memoria; il chiamante deve fornire @p OutBlock.
			 *       Non esegue validazione approfondita dei caratteri, affidandosi a @c decode_base64().
			 *
			 * @warning Non thread-safe se @c decode_base64() accede a dati globali modificabili
			 *          senza sincronizzazione.
			 *
			 * @complexity O(1) — elabora sempre un blocco fisso di 4 caratteri.
			 *
			 * @see decode_base64, base64_to_binary
			 * @since 1.0
			 */
			uint8_t block_to_binary(char* InBlock, uint8_t* OutBlock);
			/**
			 * @brief Converte un blocco di massimo 3 byte binari in 4 caratteri Base64.
			 *
			 * @details Codifica un gruppo di 1, 2 o 3 byte in una sequenza di 4 caratteri
			 *          Base64 secondo l'alfabeto presente in @c Base64Table.
			 *          Se il numero di byte in ingresso è inferiore a 3, la funzione aggiunge
			 *          caratteri di padding '=' secondo le regole di RFC 4648.
			 *
			 *          L'algoritmo:
			 *          - Estrae gruppi di 6 bit dai byte di input usando shift e mascherature.
			 *          - Usa @c Base64Table per mappare ciascun gruppo di 6 bit in un carattere ASCII.
			 *          - Gestisce i casi di input parziale (1 o 2 byte) aggiungendo i caratteri '='.
			 *
			 * @param[in]  InBlock  Puntatore a un buffer contenente fino a 3 byte di dati binari da codificare.
			 * @param[out] OutBlock Puntatore a un buffer di almeno 4 caratteri dove verrà scritta la codifica Base64.
			 * @param[in]  Bytes    Numero di byte effettivi in @p InBlock (1 ≤ Bytes ≤ 3).
			 *
			 * @return Nessun valore di ritorno. I 4 caratteri Base64 risultanti (con eventuali '=' di padding)
			 *         vengono scritti direttamente in @p OutBlock.
			 *
			 * @pre  @p InBlock deve puntare ad almeno @p Bytes byte validi.
			 *       @p OutBlock deve avere spazio per almeno 4 caratteri.
			 *       1 ≤ @p Bytes ≤ 3.
			 * @post I 4 caratteri Base64 generati sono scritti in @p OutBlock.
			 *
			 * @note La funzione non alloca memoria; il chiamante deve fornire @p OutBlock.
			 *       Utilizza l'alfabeto in @c Base64Table, che può essere configurato
			 *       (ad esempio per modalità URL-safe) prima della chiamata.
			 *
			 * @warning Non thread-safe se @c Base64Table è condivisa tra thread senza protezione.
			 *
			 * @complexity O(1) — la funzione elabora sempre un massimo di 3 byte in ingresso.
			 *
			 * @see block_to_binary, Base64Table, binary_to_base64
			 * @since 1.0
			 */
			void block_to_base64(uint8_t* InBlock, char* OutBlock, const uint32_t Bytes);
			/**
			 * @brief Converte un carattere Base64 nel corrispondente valore a 6 bit.
			 *
			 * @details Decodifica un singolo carattere codificato in Base64 (alfabeto standard o URL-safe)
			 *          restituendo il valore numerico compreso tra 0 e 63 che rappresenta i suoi 6 bit.
			 *          L'implementazione:
			 *          - Se il carattere è alfabetico maiuscolo ('A'-'Z'), restituisce valori 0–25.
			 *          - Se è alfabetico minuscolo ('a'-'z'), restituisce valori 26–51.
			 *          - Se è numerico ('0'-'9'), restituisce valori 52–61.
			 *          - Se è uno dei due caratteri speciali presenti in @c Base64Table[62] o @c Base64Table[63],
			 *            restituisce rispettivamente 62 o 63.
			 *          - In caso contrario, solleva @c std::invalid_argument.
			 *
			 *          Se viene lanciata un'eccezione, lo stato di errore viene impostato tramite @c set_error()
			 *          e la funzione restituisce 0.
			 *
			 * @param[in] C Carattere (in forma di valore @c uint8_t) da decodificare.
			 *
			 * @return Valore compreso tra 0 e 63 corrispondente al carattere Base64.
			 *         Restituisce 0 in caso di errore.
			 *
			 * @pre  @c Base64Table deve essere inizializzata correttamente.
			 * @post Nessuna modifica ai dati membri della classe, eccetto eventuale chiamata a @c set_error().
			 *
			 * @note Supporta sia l'alfabeto Base64 standard che quello URL-safe, a seconda
			 *       dello stato di @c Base64Table[62] e @c Base64Table[63].
			 *
			 * @warning Non thread-safe se @c Base64Table viene modificata da altri thread durante l'esecuzione.
			 *
			 * @complexity O(1) — il controllo avviene con poche istruzioni condizionali.
			 *
			 * @see Base64Table, block_to_binary, encode_base64
			 * @since 1.0
			 */
			const uint8_t decode_base64(const uint8_t C);
			// Binary
			/**
			 * @brief Converte un carattere esadecimale nel corrispondente valore binario (nibble).
			 *
			 * @details Interpreta un singolo carattere ASCII come cifra esadecimale e restituisce
			 *          il valore binario corrispondente (0–15).
			 *          L'implementazione:
			 *          - Se il carattere è una cifra ('0'-'9'), restituisce 0–9.
			 *          - Se è una lettera maiuscola ('A'-'F'), restituisce 10–15.
			 *          - Se è una lettera minuscola ('a'-'f'), restituisce 10–15.
			 *          - In caso di carattere non valido, lancia un'eccezione
			 *            @c std::invalid_argument.
			 *
			 *          In caso di eccezione, imposta lo stato di errore tramite @c set_error()
			 *          e restituisce 0.
			 *
			 * @param[in] In Carattere da convertire (0–9, A–F, a–f).
			 *
			 * @return Valore binario compreso tra 0 e 15 corrispondente al carattere.
			 *         Restituisce 0 in caso di errore.
			 *
			 * @pre  @p In deve essere un carattere esadecimale valido.
			 * @post Nessuna modifica allo stato dell'oggetto, eccetto eventuale chiamata a @c set_error().
			 *
			 * @note La funzione è case-insensitive per le lettere esadecimali.
			 *
			 * @warning Non valida caratteri Unicode o ASCII esteso; accetta solo codici ASCII standard.
			 *
			 * @complexity O(1) — il controllo avviene con poche istruzioni condizionali.
			 *
			 * @see two_char_to_binary, binary_to_text, text_to_binary
			 * @since 1.0
			 */			
			uint8_t char_to_binary(char In);
			/**
			 * @brief Converte un nibble (4 bit) nel corrispondente carattere esadecimale minuscolo.
			 *
			 * @details Trasforma un valore compreso tra 0 e 15 nel carattere ASCII corrispondente:
			 *          - Valori 0–9 vengono convertiti nei caratteri '0'–'9' (codici ASCII 48–57).
			 *          - Valori 10–15 vengono convertiti nei caratteri 'a'–'f' (codici ASCII 97–102).
			 *
			 *          L'output è sempre in minuscolo per le lettere esadecimali.
			 *
			 * @param[in] Nibble Valore a 4 bit (0–15) da convertire.
			 *
			 * @return Carattere ASCII rappresentante il nibble in esadecimale minuscolo.
			 *
			 * @pre  @p Nibble deve essere compreso tra 0 e 15; valori superiori generano caratteri non validi.
			 * @post Nessuna modifica allo stato dell'oggetto.
			 *
			 * @note Questa funzione è l'operazione inversa di @c char_to_binary().
			 *       Non effettua validazione dell'input oltre al range atteso.
			 *
			 * @warning Se @p Nibble > 15 il risultato non è definito nel contesto della codifica esadecimale.
			 *
			 * @complexity O(1) — esegue un singolo controllo condizionale.
			 *
			 * @see char_to_binary, binary_to_text
			 * @since 1.0
			 */
			char nibble_to_char(uint8_t In);
			/**
			 * @brief Converte due caratteri esadecimali in un byte binario.
			 *
			 * @details Interpreta una coppia di caratteri ASCII come una rappresentazione
			 *          esadecimale di un byte e restituisce il corrispondente valore binario
			 *          (0–255).
			 *          - Il primo carattere rappresenta il nibble alto (4 bit più significativi)
			 *            e viene convertito in valore tramite @c char_to_binary(), quindi
			 *            spostato a sinistra di 4 bit.
			 *          - Il secondo carattere rappresenta il nibble basso (4 bit meno significativi)
			 *            e viene convertito e combinato tramite operazione OR bitwise.
			 *
			 * @param[in] In Puntatore a un array di almeno 2 caratteri ASCII esadecimali
			 *               (0–9, A–F, a–f) da convertire.
			 *
			 * @return Valore binario (uint8_t) corrispondente alla coppia di caratteri.
			 *
			 * @pre  @p In deve puntare ad almeno due caratteri validi in formato esadecimale.
			 * @post Nessuna modifica allo stato dell'oggetto.
			 *
			 * @note Questa funzione è complementare a @c binary_to_text() e usa @c char_to_binary()
			 *       per la conversione di ciascun nibble.
			 *
			 * @warning Se uno dei caratteri non è esadecimale valido, @c char_to_binary()
			 *          imposterà lo stato di errore e restituirà 0 per quel nibble.
			 *
			 * @complexity O(1) — elabora sempre esattamente due caratteri.
			 *
			 * @see char_to_binary, text_to_binary, binary_to_text
			 * @since 1.0
			 */
			uint8_t two_char_to_binary(char* In);
			const bool is_base64(const uint8_t C);
		private:
			char* Base64Table;
		};
		//-------------------------------------------------------------------------------
		inline std::pair<uint8_t*, uint32_t> DFormatConverter::base64_to_binary(std::string Base64, bool URL, bool Padded)
		{
			return base64_to_binary(std::make_pair(const_cast<char*>(Base64.c_str()), Base64.length()), URL, Padded);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<uint8_t*, uint32_t> DFormatConverter::base64_to_binary(std::string Base64)
		{
			return base64_to_binary(Base64, false, true);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<uint8_t*, uint32_t> DFormatConverter::base64_to_binary(std::pair<char*, uint32_t>Base64)
		{
			return base64_to_binary(Base64, false, true);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<char*, uint32_t> DFormatConverter::binary_to_base64(std::pair<uint8_t*, uint32_t>Binary)
		{
			return binary_to_base64(Binary, false, true);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<char*, uint32_t> DFormatConverter::binary_to_base64(std::string Binary)
		{
			return binary_to_base64(Binary, false, true);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<char*, uint32_t> DFormatConverter::binary_to_base64(std::string Binary, bool URL, bool Padded)
		{
			return binary_to_base64(std::make_pair((uint8_t*)Binary.c_str(), Binary.length()), URL, Padded);
		}
		//-------------------------------------------------------------------------------
		inline std::pair<uint8_t*, uint32_t>DFormatConverter::base64_to_binary(std::filesystem::path InName,
			std::filesystem::path OutName)
		{
			return base64_to_binary(InName, OutName, false, true);
		}
	}
}

