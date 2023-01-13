using System;
using System.Collections.Generic;
using System.Linq;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Text;


// Ahigad Genish
// ID: 316228022
// This program simulate producers who write news into dispatcher who sort the news - (sport/regular/weather) into co editors queues.
// The Co - editors write those news to the screen manager and print them to the screen.
// The input of the program is config file which any 3 lines in this files represent :
// 1 - producer id
// 2 - bounded Queue size
// 3 - number of news.
// We want that the number of news will be greater than the bounded queue size in order to
// watch the process of producer consumer on the screen.
namespace ProducerConsumer
{
    internal class Program
    {
        // global size of the bounded queue ;
        public const int N = 10;
        static int countDone = 0;
        // shared memory - array of prucers where any producers has own bounded queue and num of news.
        //               - coEditorNews array (unbounded queue).
        //               - coEditorSports array (unbounded queue).
        //               - coEditorWeathers array (unbounded queue).
        //               - screen array (bounded queue).
        static List<Producer> producers = new List<Producer>();
        static UBQ coEditorNews = new UBQ();
        static UBQ coEditorSports = new UBQ();
        static UBQ coEditorWeathers = new UBQ();
        static BQ screen = new BQ(N);


        static void ReadFromFile(string filePath)
        {
            try
            {
                // Try to open file.
                // This ReadAllLines open the file and close it.
                string[] lines = File.ReadAllLines(filePath);
                for (int i = 0; i < lines.Count(); i += 3)
                {
                    // Build new Producer and insert to the producers array.
                    int id = int.Parse(lines[i]);
                    int qSize = int.Parse(lines[i + 1]);
                    int numOfStrings = int.Parse(lines[i + 2]);
                    Producer newP = new Producer(id++, qSize, numOfStrings);
                    producers.Add(newP);
                }

            }
            catch (FileNotFoundException ex)
            {
                Console.WriteLine("Error: " + ex.Message);
            }
            catch (IOException ex)
            {
                Console.WriteLine("Error: " + ex.Message);
            }
            return;


        }
        static void Produce(int i)
        {

            for (int k = 0; k < producers[i].NumOfString; k++)
            {
                // Generate number and create new.
                // Insert the new into the producer bounded queue.
                Random rnd = new Random();
                int s = rnd.Next(1, 4);
                string p;

                switch (s)
                {
                    case 1:
                        p = i.ToString() + " SPORTS " + k.ToString();
                        producers[i].Bq.Enqueue(p);
                        break;
                    case 2:
                        p = i.ToString() + " NEWS " + k.ToString();
                        producers[i].Bq.Enqueue(p);
                        break;
                    case 3:
                        p = i.ToString() + " WEATHERS " + k.ToString();
                        producers[i].Bq.Enqueue(p);
                        break;
                    default:
                        break;
                }
            }
            producers[i].Bq.Enqueue("DONE");
            return;
        }
        static void Dispatcher()
        {   
            // Dispactcher run on the producers queues and got their news.
            for (int i = 0; i < producers.Count; i++)
            {
                while (true)
                {
                    // Sort them into the co - editors.
                    string newNews = producers[i].Bq.Dequeue();
                    if (newNews.Contains("SPORTS"))
                    {
                        coEditorSports.Enqueue(newNews);

                    }
                    else if (newNews.Contains("NEWS"))
                    {
                        coEditorNews.Enqueue(newNews);

                    }
                    else if (newNews.Contains("WEATHERS"))
                    {
                        coEditorWeathers.Enqueue(newNews);

                    }
                    else if (newNews == "DONE")
                    {
                        break;
                    }


                }
            }
            // insert Done message to inform the co editor.
            coEditorSports.Enqueue("DONE");
            coEditorNews.Enqueue("DONE");
            coEditorWeathers.Enqueue("DONE");
            return;
        }
        static void CoEditSports()
        {
            while (true)
            {
                // pop a news and insert to the screen
                string s = coEditorSports.Dequeue();
                Thread.Sleep(100);
                screen.Enqueue(s);
                if (s.Equals("DONE"))
                    break;
            }
            return;
        }
        static void CoEditNews()
        {

            while (true)
            { 
                // pop a news and insert to the screen
                string s = coEditorNews.Dequeue();
                Thread.Sleep(100);
                screen.Enqueue(s);
                if (s.Equals("DONE"))
                    break;
            }
            return;

        }
        static void CoEditWeathers()
        {

            while (true)
            {
                // pop a news and insert to the screen
                string s = coEditorWeathers.Dequeue();
                Thread.Sleep(100);
                screen.Enqueue(s);
                if (s.Equals("DONE"))
                    break;
            }
            return;

        }



        static void ScreenManager()
        {
            // wait for co - editors to edit news and insert to the screen manager queue.
            Thread.Sleep(1000);
            while (true)
            {
                // start to represent news.
                string s = screen.Dequeue();
                if (s.Equals("DONE"))
                {
                    countDone += 1;
                }
                Console.WriteLine(s);
                // if all co-editors done - done.
                if (countDone == 3)
                {
                    break;
                }
            }
            return;

        }
        static void Main(string[] args)
        {
            // read from file and initialize producers
            string path = "config.txt";
            ReadFromFile(path);
            // call produce by thread
            for (int i = 0; i < producers.Count; i++)
            {
                int temp = i;
                Thread newT = new Thread(() => Produce(temp));
                newT.Start();
            }
            // call dispacher
            Thread dispatcherThread = new Thread(Dispatcher);
            dispatcherThread.Start();

            // call coeditors thred : 
            Thread coEditS = new Thread(CoEditSports);
            coEditS.Start();
            Thread coEditN = new Thread(CoEditNews);
            coEditN.Start();
            Thread coEditW = new Thread(CoEditWeathers);
            coEditW.Start();

            // call screen thread:
            Thread screenThread = new Thread(ScreenManager);
            screenThread.Start();

            // join for all threads
            Thread.CurrentThread.Join(5000);
            Console.WriteLine("end");
            return;

        }
    }
}


