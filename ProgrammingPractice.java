import java.io.*;
import java.sql.*;
import java.util.*;
import java.text.SimpleDateFormat;
import java.time.*;
public class ProgrammingPractice {
    private static final String connect_string = "jdbc:oracle:thin:@aloe.cs.arizona.edu:1521:oracle";
    private static Connection m_conn;

    static void validateCredentials(String user_name, String password){
        try {
            m_conn = DriverManager.getConnection(connect_string, user_name, password);
            if(m_conn == null){
                throw new Exception("Connection failed.");
            }

        } catch (Exception e){
            e.printStackTrace();
            System.exit(1);
        }
    }

    static void importSQL(InputStream in) throws SQLException{
        Scanner s = new Scanner(in);
        s.useDelimiter("(;(\r)?\n)|(--\n)");
        Statement st = null;
        try{
            st = m_conn.createStatement();
            while (s.hasNext()){
                String line = s.next();
                if (line.startsWith("/*!") && line.endsWith("*/")){
                    int i = line.indexOf(' ');
                    line = line.substring(i + 1, line.length() - " */".length());
                }
                System.out.println(line);
                if (line.trim().length() > 0){
                    st.execute(line);
                }
            }
        }
        finally{
            if (st != null) st.close();
        }
    }

    static void enterMemberInfo(){
        Scanner in = new Scanner(System.in);
        SimpleDateFormat format = new SimpleDateFormat("mmDDyy");
        System.out.print("Enter first name: ");
        String first = in.nextLine();
        System.out.print("Enter last name: ");
        String last = in.nextLine();
        System.out.print("Enter email address: ");
        String email = in.nextLine();
        System.out.print("Is paying member (T or F): ");
        String is_paying = in.nextLine();
        if(is_paying.equals("T")){
            is_paying = "y";
        } else {
            is_paying = "n";
        }

        System.out.print("Is staff member (T or F): ");
        String is_staff = in.nextLine();
        if(is_staff.equals("T")){
            is_staff = "y";
        } else {
            is_staff = "n";
        }

        System.out.print("Is problem contributor (T or F): ");
        String is_contrib = in.nextLine();
        if(is_contrib.equals("T")){
            is_contrib = "y";
        } else {
            is_contrib = "n";
        }

        System.out.print("Enter subscription start date (as MMDDYY e.g., 120219): ");
        String str_start_date = in.nextLine();
        try {
            java.util.Date parsed = format.parse(str_start_date);
            java.sql.Date start_date = new java.sql.Date(parsed.getTime());
            PreparedStatement pstmt = m_conn.prepareStatement("INSERT INTO Member VALUES(?, ?, ?, ?, ?, ?, ?)");
            pstmt.setString(1, first);
            pstmt.setString(2, last);
            pstmt.setString(3, email);
            pstmt.setString(4, is_paying);
            pstmt.setString(5, is_staff);
            pstmt.setString(6, is_contrib);
            pstmt.setDate(7, start_date);
            pstmt.executeUpdate();
            m_conn.commit();
            System.out.println("New member added.");
        } catch (Exception e){
            e.printStackTrace();
            System.out.println("Error adding new member.");
        }
    }

    static void listQualifiedPayingMembers(){
        ResultSet rs;
        SimpleDateFormat format = new SimpleDateFormat("mmDDyy");
        Calendar calendar = Calendar.getInstance();
        java.sql.Date cur_date = new java.sql.Date(calendar.getTime().getTime());
        Calendar cal = Calendar.getInstance();
        cal.setTime(cur_date);
        cal.add(Calendar.MONTH, -50);
        java.sql.Date fifty_months_ago = new java.sql.Date(cal.getTime().getTime());
        try {
            PreparedStatement pstmt = m_conn.prepareStatement("SELECT emailAddress FROM Member WHERE subscriptionStartDate <= ?");
            pstmt.setDate(1, fifty_months_ago);
            rs = pstmt.executeQuery();
            ResultSetMetaData rsmd = rs.getMetaData();
            int numCols = rsmd.getColumnCount();
            if(numCols > 0){
                System.out.println("Here are the email addresses of members who have paid more than $400.");
                while(rs.next()){
                    System.out.println(rs.getString(1));
                }
            } else {
                System.out.println("No qualified paying member exists!");
            }

        } catch (Exception e){
            e.printStackTrace();
            System.err.println("ERROR: Could not list qualified paying members.");
        }
    }

    static void addAProblemToAPool(){
        ResultSet rs;
        Scanner in = new Scanner(System.in);
        ResultSetMetaData rsmd;
        int numCols;
        Statement s;
        PreparedStatement pstmt;
        try {
            s = m_conn.createStatement();
            rs = s.executeQuery("SELECT * FROM ProblemsPool");
            rsmd = rs.getMetaData();
            numCols = rsmd.getColumnCount();
            if(numCols == 0){
                System.out.println("No problem pools exist!");
                return;
            }
            System.out.print("Enter email address for the member having relevant problem pool: ");
            String email = in.nextLine();
            pstmt = m_conn.prepareStatement("SELECT * FROM Member WHERE emailAddress = ?");
            pstmt.setString(1, email);
            rs = pstmt.executeQuery();
            rsmd = rs.getMetaData();
            numCols = rsmd.getColumnCount();
            if(numCols == 0){
                System.out.println("No member exists with this email!");
                return;
            }
            pstmt = m_conn.prepareStatement("SELECT poolName FROM ProblemsPool WHERE emailAddress = ?");
            pstmt.setString(1, email);
            rs = pstmt.executeQuery();
            rsmd = rs.getMetaData();
            numCols = rsmd.getColumnCount();
            if(numCols == 0){
                System.out.println("This member has no problem pool!");
                return;
            }
            System.out.println("Problem pools available: ");
            System.out.println();
            int count = 1;
            while(rs.next()){
                System.out.println(Integer.toString(count) + rs.getString(1));
                count++;
            }
            System.out.println();
            System.out.print("Enter the number of problem pool desired: ");
            int problem_pool_indx = in.nextInt();
            if(problem_pool_indx < 1 || problem_pool_indx > numCols){
                System.out.println("Invalid selection!");
                return;
            }
            rs.absolute(problem_pool_indx);
            String problem_pool = rs.getString(1);



        } catch (Exception e){
            e.printStackTrace();
        }
    }

    static void mainLoop(){
        int option;
        Scanner in = new Scanner(System.in);
        System.out.println("1) Enter member Information");
        System.out.println("2) List the member(s) who paid more than $400");
        System.out.println("3) Add a problem into a specified problem pool");
        System.out.println("4) List the problems and problem pools that each member has");
        System.out.println("5) Quit");
        System.out.println();
        while(true){
            System.out.println();
            System.out.print("Enter 1 - 4 or 5 to quit: ");
            option = in.nextInt();
            if(option == 1){
                //enter member information
                enterMemberInfo();
            } else if(option == 2){
                //list members who have paid over $400
                listQualifiedPayingMembers();
            } else if(option == 3){
                //add a problem
            } else if(option == 4){
                //list problems from a pool
            } else if(option == 5){
                return;
            } else {
                System.out.println("Invalid option. Please choose a number between 1 and 5");
            }
        }
    }

    public static void main(String[] args){
        if(args.length < 2) {
            System.err.println("Oracle username and password required.");
            System.exit(2);
        }

        String user_name = args[0];
        String password = args[1];
        try {
            Class.forName("oracle.jdbc.OracleDriver");
            validateCredentials(user_name, password); //connect to the database
            File createTables = new File("./create.sql");
            File dropTables = new File("./destroy.sql");
            InputStream sqlReader = new FileInputStream(createTables);
            importSQL(sqlReader);   //create tables
            mainLoop(); //start the main program



            sqlReader = new FileInputStream(dropTables);
            importSQL(sqlReader);   //drop tables
        } catch(Exception e){
            e.printStackTrace();
            try{
                File dropTables = new File("./destroy.sql");
                InputStream sqlReader = new FileInputStream(dropTables);
                importSQL(sqlReader);   //drop tables
            } catch (Exception t){
                System.out.println("FAILED TO DROP TABLES");
                System.exit(1);
            }
            System.exit(1);
        }

    }
}
