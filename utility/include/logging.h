/**
 * @file loggin.cpp
 * @brief Small logging module
 * @author Ivanin V.A.
 * 
 * Hand written module for logging. To enable use -DLOG key in compiler args
 *
 * Author : Ivanin Vitaly
 * E-mail : alloky2011@gmail.com
 * 
 */



#include <iostream>
/**
 * @brief info debug level
 */
#define LL_INFO "INFO"

/**
 * @brief error debug level
 */
#define LL_ERR "ERROR"

/**
 * @brief warning debug level
 */
#define LL_WARN "WARNING"

/**
 * @brief debug debug level
 */
#define LL_DEBUG "DEBUG"
#ifdef LOGMOD


#define LEV_LOG(level, text) if(1) std::cout << level << " : " << text << "\n"
#define LOG(text) if(1) std::cout << ">> " << text << "\n"

#else

/**
 * @def LEV_LOG(text, level)
 *
 * @brief      Write log to console with level.
 *
 * @param      text   logging message
 * @param      level  level of log
 *
 * \b Warrning: To enable output compile with -DLOG flag
 *  
 * Write log to console with level. Level is LL_* define.
 * 
 * See example : @include lev_log_exmpl.cpp
 */
#define LEV_LOG(level, text) if(0) std::cout << level << " : " << text << "\n"  


/**
 * @def LOG(text)
 * 
 * @brief      Logging without levels
 *
 * @param      text  Logging message
 *
 * \b Warrning: To enable output compile with -DLOG flag
 * 
 * Write to std::cout ">> {text}".
 * 
 * See example : @include log_exmpl.cpp
 */
#define LOG(text) if(0) std::cout << ">> " << text << "\n"

#endif
