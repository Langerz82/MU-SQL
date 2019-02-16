

#ifndef _MU_GAMESOCKET_H
#define _MU_GAMESOCKET_H

#include "StdAfx.h"
#include "Common.h"
#include "Auth/BigNumber.h"
#include "Auth/Sha1.h"
#include "ByteBuffer.h"
#include "Utilities/Util.h"

#include "SocketBuffer/BufferedSocket.h"

class ACE_INET_Addr;


class GameSocket: public BufferedSocket
{
    public:
        const static int s_BYTE_SIZE = 32; /**< TODO */

        /**
         * @brief
         *
         */
		GameSocket();
        /**
         * @brief
         *
         */
        ~GameSocket();

        /**
         * @brief
         *
         */
        void OnAccept() override;
        /**
         * @brief
         *
         */
        void OnRead() override;
        /**
         * @brief
         *
         * @param sha
         */
        void SendProof(Sha1Hash sha);

        static ACE_INET_Addr const& GetAddressForClient(ACE_INET_Addr const& clientAddr);

        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleLogonChallenge();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleLogonProof();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleReconnectChallenge();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleReconnectProof();

        /**
         * @brief data transfer handle for patch
         *
         * @return bool
         */
        bool _HandleXferResume();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleXferCancel();
        /**
         * @brief
         *
         * @return bool
         */
        bool _HandleXferAccept();

        /**
         * @brief
         *
         * @param rI
         */
        void _SetVSFields(const std::string& rI);

    private:
        enum eStatus
        {
            STATUS_CHALLENGE,
            STATUS_LOGON_PROOF,
            STATUS_RECON_PROOF,
            STATUS_PATCH,
            STATUS_AUTHED,
            STATUS_CLOSED
        };

        BigNumber N, s, g, v; /**< TODO */
        BigNumber b, B; /**< TODO */
        BigNumber K; /**< TODO */
        BigNumber _reconnectProof; /**< TODO */

        eStatus _status; /**< TODO */

        std::string _login; /**< TODO */
        std::string _safelogin; /**< TODO */

        std::string _os;
        uint16 _build; /**< TODO */

        ACE_HANDLE patch_; /**< TODO */
};
#endif
/// @}
