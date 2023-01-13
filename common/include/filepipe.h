#ifndef FILEPIPE_H
#define FILEPIPE_H
#include <string>
#include <fstream>
#include "../../common/include/ftpstatics.h"

/** @brief file sender between FtpServer and FtpClient
 * this class is generalized and can be used for both server/client and both sender/reciver.
 */
class FilePipe
{
private:
    int dataPort,dataFd;
    int serverFd;
    int userFd;
    std::string path;
    std::fstream file;
    char fileBuffer[FILE_PIPE_BUFFER_SIZE];
    bool firstBlock;
    int debugDelayInterval;

    void reciverRun(); /**< @brief blocking run of reciver mode*/
    void senderRun(); /**< @brief blocking run of sender mode*/
    bool setupServer(); /**< @brief setup file pipe as server*/
    bool setupClient(); /**< @brief setup file pipe as client*/

    int sendNextBlock(); /**< @brief send next block of data in sender mode*/
    int reciveNextBlock(); /**< @brief recive next block of data in reciver mode*/
    void sendAck(); /**< @brief reciver use this method to ask next block of data from the sender*/
    void reciveAck(); /**< @brief a blocking method that sender use it to read request of next block from reciver*/
public:
    enum Dir{
        sender,reciver
    }dir;

    enum Role{
        server,client
    }role;

    /**< @brief FilePipe constructor
    * @param pipeRole determines being server or client
    * @param pipeDir determines being sender or reciver
    * @param file path for readinor/storing
    */
    FilePipe(Role pipeRole, Dir pipeDir, std::string filePath);

    bool setup(int port);
    void endConnection();
    void run();
    int eventloop();
    int getServerFd() const;
    int getDataFd() const;
    void debugDelay();
    int getDataPort() const;
    int getUserFd() const;
    void setUserFd(int newUserFd);
    void setDebugDelay(int newDebugDelay);
    Dir getDir() const;
};

#endif // FILEPIPE_H
