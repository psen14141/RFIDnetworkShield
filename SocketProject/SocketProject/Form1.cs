using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Threading;

namespace SocketProject
{
    public partial class Form1 : Form
    {
        bool sendMessage;
        Socket clientSocket;

        public Form1()
        {
            InitializeComponent();
        }
     
        private void button1_Click(object sender, EventArgs e)
        {
            (new Thread(ExecuteServer)).Start();
            (new Thread(ServerHartbeet)).Start();
        }

        public void ServerHartbeet()
        {
            while (true)
            {
                SendClientMessage("Server Hartbeat\n");
                Thread.Sleep(5000);
            }
        }

        public void ExecuteServer()
        {
            // Establish the local endpoint  
            // for the socket. Dns.GetHostName 
            // returns the name of the host  
            // running the application. 
            IPHostEntry ipHost = Dns.GetHostEntry(Dns.GetHostName());
            IPAddress ipAddr = ipHost.AddressList[ipHost.AddressList.Length-1];


            IPEndPoint localEndPoint = new IPEndPoint(ipAddr, 2020);

            // Creation TCP/IP Socket using  
            // Socket Class Costructor 
            Socket listener = new Socket(ipAddr.AddressFamily,
                         SocketType.Stream, ProtocolType.Tcp);

            try
            {

                // Using Bind() method we associate a 
                // network address to the Server Socket 
                // All client that will connect to this  
                // Server Socket must know this network 
                // Address 
                listener.Bind(localEndPoint);

                // Using Listen() method we create  
                // the Client list that will want 
                // to connect to Server 
                listener.Listen(10);

                while (true)
                {

                    Console.WriteLine("Waiting connection ... ");

                    // Suspend while waiting for 
                    // incoming connection Using  
                    // Accept() method the server  
                    // will accept connection of client 
                    clientSocket = listener.Accept();

                    // Data buffer 
                    byte[] bytes = new Byte[1024];
                    string data = null;

                    while (true)
                    {

                        int numByte = clientSocket.Receive(bytes);

                        data = Encoding.ASCII.GetString(bytes,0, numByte);

                        if (numByte > 0)
                        {
                            SetControlText(  "Read >> "+ data);
                        }

                        ProcessIncomingCommand(data);

                        if (data.IndexOf("<EOF>") > -1)
                            break;

                       
                    }

                    Console.WriteLine("Text received -> {0} ", data);
                    byte[] message = Encoding.ASCII.GetBytes("Test Server");

                    // Send a message to Client  
                    // using Send() method 
                    clientSocket.Send(message);

                    // Close client Socket using the 
                    // Close() method. After closing, 
                    // we can use the closed Socket  
                    // for a new Client Connection 
                    clientSocket.Shutdown(SocketShutdown.Both);
                    clientSocket.Close();
                }
            }

            catch (Exception e)
            {
                Console.WriteLine(e.ToString());

                SetControlText( "Server Closed >> " + e.Message);

            }
        }

        private void ProcessIncomingCommand(string data)
        {
            if (data.Contains("Card[9577966]"))
            {
                SendClientMessage("BuzerGood");
            }
            else if(data.Contains("Card["))
            {
                SendClientMessage("BuzerWarning");
            }
        }

        private void SetControlText(string text)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new Action<string>(SetControlText), new object[] { text });
            }
            else
            {
                labelServerStatus.Text = labelServerStatus.Text +Environment.NewLine+ text;
                labelServerStatus.ScrollToCaret();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SendClientMessage(comboBoxCommand.Text);
        }

        private void SendClientMessage(string message)
        {
            try
            {
                byte[] msg = Encoding.ASCII.GetBytes(message);
                clientSocket.Send(msg);
                SetControlText("Write >> " + message);
            }
            catch (Exception)
            {

            }
           
        }

        private void ButtonClear_Click(object sender, EventArgs e)
        {
            labelServerStatus.Text = "";
        }
    }
} 



